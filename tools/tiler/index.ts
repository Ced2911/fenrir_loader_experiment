import Jimp from 'jimp';
import { writeFile } from 'fs';
import { createHash } from 'crypto';

export interface RGBA {
    r: number;
    g: number;
    b: number;
    a: number;
}
export interface TILE {
    hash: string;
    data: number[];
}

const TILE_W = 8;
const TILE_H = 8;

const tiles: Record<string, TILE> = {}
const map: number[] = []

function buildPalette(image: Jimp) {
    const palette: Record<number, RGBA> = {}
    palette[0] = { r: 0, g: 0, b: 0, a: 1 };
    image.scan(0, 0, image.bitmap.width, image.bitmap.height, function (x, y, idx) {
        var red = this.bitmap.data[idx + 0];
        var green = this.bitmap.data[idx + 1];
        var blue = this.bitmap.data[idx + 2];
        var alpha = this.bitmap.data[idx + 3];

        var pixel = Jimp.rgbaToInt(red, green, blue, alpha);

        if (palette[pixel] === undefined) {
            palette[pixel] = { r: red, g: green, b: blue, a: alpha }
        }
    });
    return Object.values(palette)
}

function buildImage(image: Jimp, paletteArray: RGBA[]) {
    const data: number[] = [];
    image.scan(0, 0, image.bitmap.width, image.bitmap.height, function (x, y, idx) {
        const red = this.bitmap.data[idx + 0];
        const green = this.bitmap.data[idx + 1];
        const blue = this.bitmap.data[idx + 2];
        const alpha = this.bitmap.data[idx + 3];
        const rgba = { r: red, g: green, b: blue, a: alpha }

        const pixel = Jimp.rgbaToInt(red, green, blue, alpha);
        const pal = paletteArray.findIndex((t) => { return pixel === Jimp.rgbaToInt(t.r, t.g, t.b, t.a) })

        data.push(pal)
    });

    return data
}

function tileImage(image: Jimp, paletteArray: RGBA[], cbk: Function) {
    const tiles: number[][] = [];

    for (let y = 0; y < image.bitmap.height; y += 8) {
        for (let x = 0; x < image.bitmap.width; x += 8) {

            const data: number[] = [];

            image.scan(x, y, 8, 8, function (x, y, idx) {
                const red = this.bitmap.data[idx + 0];
                const green = this.bitmap.data[idx + 1];
                const blue = this.bitmap.data[idx + 2];
                const alpha = this.bitmap.data[idx + 3];

                const pixel = Jimp.rgbaToInt(red, green, blue, alpha);
                const pal = pixel == 0 ? 0 : paletteArray.findIndex((t) => { return pixel === Jimp.rgbaToInt(t.r, t.g, t.b, t.a) })

                data.push(pal)
            });

            cbk(data, tiles.length, x / 8, y / 8);
            tiles.push(data)
        }
    }

    return tiles;
}

function RGB8888To555(rgba: RGBA) {
    return `COLOR_RGB1555(${rgba.a ? 1 : 0}, ${Math.floor(rgba.r / 8)}, ${Math.floor(rgba.g / 8)}, ${Math.floor(rgba.b / 8)})`
}

const argv = [...process.argv]
const key = argv.pop()
const output = argv.pop()
const input = argv.pop()

Jimp.read(input)
    .then(image => {
        const pal = buildPalette(image)
        const img = buildImage(image, pal)
        const pattern: number[] = []
        const pages: Record<number, number[]> = { 0: [], 1: [], 2: [], 3: [] }
        const cells: Record<string, number[]> = {}
        let i = 0;
        tileImage(image, pal, (cellData, tilen, x, y) => {
            const d = createHash('sha256').update(Buffer.from(cellData)).digest('hex');
            cells[d] = cellData;
            const ptn = Object.keys(cells).findIndex((h) => d == h)

            // convert to ss fmt
            const page = x > 63 ? 1 : 0 + y > 63 ? 2 : 0;
            const addr = ptn * 64;
            const ptn_ss = ((addr + 0) >> 5);
            //pattern.push(ptn_ss)
            pages[page].push(ptn_ss)

            i++
        })

        pattern.push(...Object.values(pages).flatMap(x => x))

        let k = key;

        let patternstr = `
static const unsigned long ${k}_pattern_sz = ${pattern.length}*sizeof(unsigned short);
static const unsigned short ${k}_pattern[] = {
    ${pattern.join(',')}
};`
        let cellstr = `
static const unsigned long ${k}_cell_sz = ${Object.values(cells).length * 8 * 8}*sizeof(unsigned char);
static const unsigned char ${k}_cell[] = {
    ${Object.values(cells).flatMap(x => x).join(',')}
};`

        let palstr = `
static const unsigned long ${k}_pal_sz = ${Object.values(pal).length}*sizeof(color_rgb1555_t);
static const color_rgb1555_t ${k}_pal[] = {
    ${Object.values(pal).map(c => RGB8888To555(c)).join(',')}
};`
        //console.log(tilesstr)
        writeFile(output, `// Auto generated\n\n` + palstr + patternstr + cellstr, () => { })
    })
    .catch(err => {
        console.error(err);
    });