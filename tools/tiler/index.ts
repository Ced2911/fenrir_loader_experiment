import Jimp from 'jimp';
import { writeFile } from 'fs';

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

function RGB8888To555(rgba: RGBA) {
    return `COLOR_RGB1555(${rgba.a ? 1 : 0}, ${Math.floor(rgba.r / 8)}, ${Math.floor(rgba.g / 8)}, ${Math.floor(rgba.b / 8)})`
}

Jimp.read('../../assets/bitmap.png')
    .then(image => {
        const pal = buildPalette(image)
        const img = buildImage(image, pal)


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
        writeFile('../../assets/test.h', tilesstr + palstr, () => { })
    })
    .catch(err => {
        console.error(err);
    });