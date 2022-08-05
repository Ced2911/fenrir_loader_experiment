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
                const rgba = { r: red, g: green, b: blue, a: alpha }

                const pixel = Jimp.rgbaToInt(red, green, blue, alpha);
                const pal = paletteArray.findIndex((t) => { return pixel === Jimp.rgbaToInt(t.r, t.g, t.b, t.a) })

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

Jimp.read('../../assets/mvsc_bg.png')
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


        console.log(pattern.length);
        console.log(Object.values(cells).length)

        let patternstr = `
const unsigned long patternLength = ${pattern.length}*sizeof(unsigned short);
const unsigned short pattern[] = {
    ${pattern.join(',')}
};`
        let cellstr = `
const unsigned long cellsLength = ${Object.values(cells).length * 8 * 8}*sizeof(unsigned char);
const unsigned char cells[] = {
    ${Object.values(cells).flatMap(x => x).join(',')}
};`

        let tilesstr = `
const unsigned long bitmapLength = ${img.length}*sizeof(unsigned char);
const unsigned char bitmap[] = {
    ${img.join(',')}
};`

        let palstr = `
const unsigned long palLength = ${Object.values(pal).length}*sizeof(color_rgb1555_t);
const color_rgb1555_t pal[] = {
    ${Object.values(pal).map(c => RGB8888To555(c)).join(',')}
};`
        //console.log(tilesstr)
        writeFile('../../assets/test.h', tilesstr + palstr + patternstr + cellstr, () => { })
    })
    .catch(err => {
        console.error(err);
    });