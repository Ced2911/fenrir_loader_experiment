import Jimp from 'jimp';
import { writeFile } from 'fs';
import { createHash } from 'crypto';
import { buildPalette, tileImage, RGBA, RGB8888To555 } from './tiler'

interface CONFIG_IMAGE {
    key: string,
    file: string
}
interface CONFIG {
    images: CONFIG_IMAGE[],
    output: string
}
interface CELL {
    data: number[],
    id: number
}

const config = require('./config.json') as CONFIG;

// console.log(config)

async function main() {
    // contain all tiles for all screens
    const cells: Map<string, CELL[]> = new Map()
    const cellsPerScreen: Record<string, number> = {}

    // pattern for each file/screen
    const pattern: Record<string, number[]> = {}

    // palette for each file/screen
    const palettes: Record<string, RGBA[]> = {}

    // always add a transparent tile
    {
        const cellData = (new Array(8 * 8)).map(v => 0)
        const hash = createHash('sha256').update(Buffer.from(cellData)).digest('hex');
        cells[hash] = { data: cellData, id: 0 };
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
                const screenHash = {}
                tileImage(image, palettes[key], (cellData) => {
                    const hash = createHash('sha256').update(Buffer.from(cellData)).digest('hex');
                    cells[hash] = { data: cellData, id: cells.size };
                    screenHash[hash] = hash;
                })
                cellsPerScreen[key] = Object.values(screenHash).length;
            })

        // 3rd pattern
        await Jimp.read(file)
            .then(image => {
                const pages: Record<number, number[]> = { 0: [], 1: [], 2: [], 3: [] }

                tileImage(image, palettes[key], (cellData, tilen, x, y) => {
                    const hash = createHash('sha256').update(Buffer.from(cellData)).digest('hex');

                    const ptn = Object.keys(cells).findIndex((h) => hash == h)

                    // convert to ss fmt - cfg 1
                    const page = x > 63 ? 1 : 0 + y > 63 ? 2 : 0;
                    const addr = ptn * 64;
                    const ptn_ss = ((addr + 0) >> 5);

                    pages[page].push(ptn_ss)
                })

                pattern[key] = Object.values(pages).flatMap(x => x)
            })
    }))

    // output stats
    console.log(`number of global tiles: ${Object.values(cells).length}`)
    console.log(`number of global pattern: ${Object.values(pattern).reduce((acc, pscreen) => acc + pscreen.length, 0)}`)
    console.log(`number of global palettes: ${Object.values(pattern).reduce((acc, pscreen) => acc + pscreen.length, 0)}`)

    config.images.map(({ key, file }) => {
        console.log('\n')
        console.log(`file: ${file}`)
        console.log(`\tnumber of ${key} tiles: ${cellsPerScreen[key]}`)
        console.log(`\tnumber of ${key} pattern: ${pattern[key].length}`)
        console.log(`\tnumber of ${key} palettes: ${palettes[key].length}`)
    })

    // output..
    let cellstr = `
    // ${Object.values(cells).length} cells
    static const size_t shared_cell_sz = ${Object.values(cells).map(cell => cell.data).length * 8 * 8}*sizeof(uint8_t);
    static const uint8_t shared_cell[] = {
        ${Object.values(cells).map(cell => cell.data).flatMap(x => x).join(',')}
    };`
    let patternStr = '';
    let palStr = '';
    config.images.map(({ key, file }) => {
        patternStr += `
static const size_t ${key}_pattern_sz = ${pattern[key].length}*sizeof(uint16_t);
static const uint16_t ${key}_pattern[] = {
    ${pattern[key].join(',')}
};`

        palStr += `
static const size_t ${key}_pal_sz = ${Object.values(palettes[key]).length}*sizeof(color_rgb1555_t);
static const color_rgb1555_t ${key}_pal[] = {
    ${Object.values(palettes[key]).map(c => RGB8888To555(c)).join(',')}
};`
    })

    // write to file    
    writeFile(config.output, `// Auto generated\n//${config.images.map(({ key, file }) => file).join('\n//')}\n` + palStr + patternStr + cellstr, () => { })
}

main();