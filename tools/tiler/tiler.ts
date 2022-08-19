import Jimp from 'jimp';
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

export function buildPalette(image: Jimp) {
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

export function tileImage(image: Jimp, paletteArray: RGBA[], cbk: Function) {
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
                const pal = alpha == 0 ? 0 : paletteArray.findIndex((t) => { return pixel === Jimp.rgbaToInt(t.r, t.g, t.b, t.a) })

                data.push(pal)
            });

            cbk(data, tiles.length, x / 8, y / 8);
            tiles.push(data)
        }
    }

    return tiles;
}

export function RGB8888To555(rgba: RGBA) {
    return `COLOR_RGB1555(${rgba.a ? 1 : 0}, ${Math.floor(rgba.r / 8)}, ${Math.floor(rgba.g / 8)}, ${Math.floor(rgba.b / 8)})`
}
