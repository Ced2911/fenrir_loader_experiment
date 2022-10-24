import Jimp from 'jimp';
import { writeFile } from 'fs';
import { createHash } from 'crypto';
import { buildPalette, tileImage, RGBA, TileSize, Cell } from './tiler'
import { exportBufferToC, exportPaletteToBin, exportCellsToBin, exporPtnToBin, cellPattern, patternGetPage, vdpNextPatternAddrBoundary, createVdpConfig } from './encode'

interface ConfigImage {
    key: string,
    file: string,
    screen: 'nbg0' | 'nbg2'
}
interface Config {
    images: ConfigImage[],
    output: string
}

enum CellMirror {
    N = 'n',
    Vertical = 'v',
    Horizontal = 'h',
    Both = 'vh'
}

const config = require('./config.json') as Config;

// console.log(config)

function flipCellData(data: number[], mirror: number): number[] {
    switch (mirror) {
        case 1: // v
            {
                const mirrorData: number[] = new Array(data.length)
                for (let x = 0; x < 8; x++) {
                    for (let y = 0; y < 8; y++) {
                        mirrorData[x + (y * 8)] = data[x + ((7 - y) * 8)];
                    }
                }
                return mirrorData;
            }
        case 2: //h
            {
                const mirrorData: number[] = new Array(data.length)
                for (let x = 0; x < 8; x++) {
                    for (let y = 0; y < 8; y++) {
                        mirrorData[x + (y * 8)] = data[7 - x + (y * 8)];
                    }
                }
                return mirrorData;
            }
        case 3: // hv
            return Array.from(data).reverse();
        default:
            return data;
    }
}

function hashCell(cell: Cell) {
    for (let i = 0; i < 4; i++) {
        const flippedCell = flipCellData(cell.data, i)
        const hash = createHash('sha256').update(Buffer.from(flippedCell)).digest('hex');
        cell.hash[i] = hash
    }
}


async function main() {
    const cellSize: TileSize = TileSize.x1

    // contain all tiles for all screens
    const cells: Record<string, Cell> = {}
    const cellsLut: Record<string, Record<string, { id: number }>> = {
        'n': {},
        'v': {},
        'h': {},
        'vh': {}
    }
    const cellsPerScreen: Record<string, number> = {}

    // pattern for each file/screen
    const pattern: Record<string, number[]> = {}

    // palette for each file/screen
    const palettes: Record<string, RGBA[]> = {}

    function addCell(cell: Cell) {
        const hash = createHash('sha256').update(Buffer.from(cell.data)).digest('hex');

        // try to find a cell, 
        // if not found compute hash of all mirros and luts
        if (!findCell(hash)) {
            hashCell(cell)
            cells[hash] = cell;

            // add luts...
            if (1)
                for (let i = 0; i < 4; i++) {
                    const m = [CellMirror.N, CellMirror.Vertical, CellMirror.Horizontal, CellMirror.Both][i]
                    const cell_m = { id: cell.id }
                    cellsLut[m][cell.hash[i]] = cell_m
                }


            return 1;
        }
        return 0;
    }

    function findCell(hash: string) {
        if (cells[hash]) {
            return { id: cells[hash].id, mirror: 0 }
        }
        if (1)
            // check mirrored
            for (let i = 0; i < 4; i++) {
                const m = [CellMirror.N, CellMirror.Vertical, CellMirror.Horizontal, CellMirror.Both][i]
                const cell = cellsLut[m][hash]
                if (cell) {
                    //console.log('found mirrored !', cell.id, i)
                    return { id: cell.id, mirror: i }
                }
            }

        return undefined
    }

    // always add a transparent tile
    {
        const cellData = new Array(8 * 8).fill(0)
        const hash = createHash('sha256').update(Buffer.from(cellData)).digest('hex');
        cells[hash] = { data: cellData, id: 0, hash: [hash, hash, hash, hash] };
    }

    await Promise.all(config.images.map(async ({ key, file }) => {

        // 1st step build palettes
        await Jimp.read(file)
            .then(image => {
                palettes[key] = buildPalette(image)
            })

        // 2nd unik tiles
        await Jimp.read(file)
            .then(image => {
                let unikcell = 0;
                tileImage(image, cellSize, palettes[key], (cellData) => {
                    const cell: Cell = {
                        data: cellData,
                        id: Object.values(cells).length,
                        hash: []
                    }
                    unikcell += addCell(cell)
                })
                cellsPerScreen[key] = unikcell;
            })

        // 3rd pattern
        await Jimp.read(file)
            .then(image => {
                const pages: Record<number, number[]> = { 0: [], 1: [], 2: [], 3: [] }

                tileImage(image, cellSize, palettes[key], (cellData, tilen, x, y) => {
                    const hash = createHash('sha256').update(Buffer.from(cellData)).digest('hex');

                    const cell = findCell(hash)
                    if (!cell)
                        throw (`cell for hash: ${hash} not`)

                    const p = cellPattern(cell)
                    pages[patternGetPage(x, y)].push(p)
                })

                pattern[key] = Object.values(pages).flatMap(x => x)
            })
    }))

    // output stats
    console.log(`number of global tiles: ${Object.values(cells).length} 0x${Object.values(cells).length.toString(16)}`)
    console.log(`number of global pattern: ${Object.values(pattern).reduce((acc, pscreen) => acc + pscreen.length, 0)}`)
    console.log(`number of global palettes: ${Object.values(pattern).reduce((acc, pscreen) => acc + pscreen.length, 0)}`)


    config.images.map(({ key, file }) => {
        console.log('\n')
        console.log(`file: ${file}`)
        if (cellsPerScreen[key] > 0x0200) {
            console.warn("\tcells are bigger than 0x200 expect some errors")
        }
        console.log(`\tnumber of ${key} cells: ${cellsPerScreen[key]}`)
        console.log(`\tnumber of ${key} pattern: ${pattern[key].length}`)
        console.log(`\tnumber of ${key} palettes: ${palettes[key].length}`)
    })

    // output..
    const screenConfig: any = {
        screens: []
    }


    let cellstr = '';
    const cellbin = exportCellsToBin(Object.values(cells));
    //console.log('cell', vdpNextPatternAddrBoundary(cellbin.length))
    cellstr += exportBufferToC(`shared_cell`, cellbin)

    screenConfig.cell = cellbin.length

    let patternStr = '';
    let palStr = '';
    config.images.map(({ key, file, screen }) => {
        const patternbin = exporPtnToBin(pattern[key])
        const palbin = exportPaletteToBin({ palettes: Object.values(palettes[key]) })

        //console.log('patternbin', vdpNextPatternAddrBoundary(patternbin.length))
        //console.log('palbin', vdpNextPatternAddrBoundary(palbin.length))

        patternStr += exportBufferToC(`${key}_pattern`, patternbin);
        palStr += exportBufferToC(`${key}_pal`, palbin);

        screenConfig.screens[screen] = screenConfig.screens[screen] || {}

        screenConfig.screens[screen].pattern = patternbin.length
        screenConfig.screens[screen].pal = palbin.length

    })

    const vdp2config = createVdpConfig(screenConfig)

    // write to file    
    writeFile(config.output, `// Auto generated\n//${config.images.map(({ key, file }) => file).join('\n//')}\n` + palStr + patternStr + cellstr + vdp2config, () => { })
}

main();