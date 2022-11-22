import Jimp from 'jimp';
import { writeFile } from 'fs';
import { createHash } from 'crypto';
import { buildPalette, tileImage, RGBA, TileSize, Cell, Cell8bppTo4bpp } from './tiler'
import { cell4pbbPattern, cellPattern, patternGetPage, Vdp2ColorCnt, VDP2Memory, Vdp2Screen, vdpNextPatternAddrBoundary } from './encode'
import { type } from 'os';

interface ConfigImage {
    key: string,
    file: string,
    screen: Vdp2Screen
}
export interface Config {
    images: ConfigImage[],
    output: string
}

enum CellMirror {
    N = 'n',
    Vertical = 'v',
    Horizontal = 'h',
    Both = 'vh'
}

export type VDP2ScreenConfig = Record<string, any>

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


export async function tileVdp2(config: Config,
    vdp2: VDP2Memory,
    vdp2ScreenConfig: VDP2ScreenConfig) {
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
    const pattern_offset_per_key: Record<string, number> = {}

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
            if (0)
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
        if (0)
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

    let bpp = Vdp2ColorCnt.color8bpp

    //await Promise.all(config.images.map(async ({ screen, file }) => {
    for (const { screen, file } of config.images) {

        let palette: RGBA[] = []
        palettes[screen] = palettes[screen] || []
        pattern[screen] = pattern[screen] || []

        // 1st step build palettes
        await Jimp.read(file)
            .then(image => {
                palette = buildPalette(image)
            })

        const paletteUniques: Map<number, RGBA> = new Map;
        [...palette, ...palettes[screen]].forEach(p => {
            paletteUniques.set(p.pixel, p)
        })

        palettes[screen] = Array.from(paletteUniques.values())

        // rebuild ids
        palettes[screen].map((p, i) => p.id = i)
        //}))
    }

    //const all4bpp = (Object.values(palettes)).every(c => c.length < 16)
    const all4bpp = 0
    bpp = all4bpp ? Vdp2ColorCnt.color4bpp : Vdp2ColorCnt.color8bpp

    //await Promise.all(config.images.map(async ({ screen, file }) => {
    for (const { screen, file } of config.images) {
        // 2nd unik tiles
        await Jimp.read(file)
            .then(image => {
                let unikcell = 0;
                tileImage(image, cellSize, palettes[screen], (cellData) => {
                    const cell: Cell = {
                        data: cellData,
                        id: Object.values(cells).length,
                        hash: []
                    }
                    unikcell += addCell(cell)
                })
                cellsPerScreen[screen] = unikcell;
            })
        //}))
    }

    //await Promise.all(config.images.map(async ({ screen, key, file }) => {
    for (const { screen, key, file } of config.images) {
        // 3rd pattern
        await Jimp.read(file)
            .then(image => {
                const pages: Record<number, number[]> = { 0: [], 1: [], 2: [], 3: [] }

                tileImage(image, cellSize, palettes[screen], (cellData, tilen, x, y) => {
                    const hash = createHash('sha256').update(Buffer.from(cellData)).digest('hex');

                    const cell = findCell(hash)
                    if (!cell)
                        throw (`cell for hash: ${hash} not`)

                    const p = palettes[screen].length < 16 ? cell4pbbPattern(cell) : cellPattern(cell)
                    pages[patternGetPage(x, y)].push(p)
                })

                // need padding ?
                let len = pattern[screen].length;
                if (len > 0) {
                    // align to next 1<<12
                    len = (len + 4095) & (~4095);
                    const off = len
                    //  console.log('len', len, pattern[screen].length, 'd', off, off - pattern[screen].length)
                    // add padding
                    pattern[screen].push(...Array(off - pattern[screen].length))
                }
                pattern_offset_per_key[key] = len * 4 // 1word

                pattern[screen].push(...Object.values(pages).flatMap(x => x))
            })
        //}))
    }

    // build binaries
    const sharedCell = Object.values(cells).flatMap(cell => cell.data)
    vdp2.addSharedCells(sharedCell, bpp)
    config.images.map(({ screen }) => {
        vdp2.addCharPattern(screen, pattern[screen])
        vdp2.addPalette(screen, { palettes: Object.values(palettes[screen]) })
    })

    // console.log(palettes)

    console.log(`use ${Object.values(cells).length} cells`)

    //const cfg = Object.entries(pattern_offset_per_key).filter(([k, l]) => l !== undefined).map(([k, l]) => `static const uint32_t pattern_offset_${k}=${l};`).join('\n')

    Object.entries(pattern_offset_per_key)
        .filter(([k, l]) => l !== undefined)
        .forEach(([k, l]) => vdp2ScreenConfig[k] = l)

    // write to file    
    // writeFile(config.output, `// Auto generated\n//${config.images.map(({ key, file }) => file).join('\n//')}\n` + cfg + vdp2c, () => { })
}
