import Jimp from 'jimp';
import { createHash } from 'crypto';

export interface RGBA {
    r: number;
    g: number;
    b: number;
    a: number;
    id: number;
    pixel: number;
}
export interface TILE {
    hash: string;
    data: number[];
}
export enum TileSize {
    x1 = 8,
    x2 = 16
}

type tileImageCallback = (cellData: number[], tilesId: number, tileX: number, tileY: number) => void;

export function buildPalette(image: Jimp) {
    const palette: Map<number, RGBA> = new Map
    // always transparent first
    palette.set(0, { r: 0, g: 0, b: 0, a: 0, id: 0, pixel: 0 });

    image.scan(0, 0, image.bitmap.width, image.bitmap.height, function (x, y, idx) {
        var red = this.bitmap.data[idx + 0];
        var green = this.bitmap.data[idx + 1];
        var blue = this.bitmap.data[idx + 2];
        var alpha = this.bitmap.data[idx + 3];

        var pixel = (alpha == 0) ? 0 : Jimp.rgbaToInt(red, green, blue, alpha);

        if (palette.get(pixel) === undefined) {
            palette.set(pixel, { r: red, g: green, b: blue, a: alpha, id: palette.size, pixel: pixel })
        }
    });

    return Array.from(palette.values())
}

export function tileImage(image: Jimp, tileSize: TileSize, paletteArray: RGBA[], cbk: tileImageCallback) {
    const tiles: number[][] = [];

    for (let y = 0; y < image.bitmap.height; y += tileSize) {
        for (let x = 0; x < image.bitmap.width; x += tileSize) {

            const data: number[] = [];

            image.scan(x, y, 8, 8, function (x, y, idx) {
                const red = this.bitmap.data[idx + 0];
                const green = this.bitmap.data[idx + 1];
                const blue = this.bitmap.data[idx + 2];
                const alpha = this.bitmap.data[idx + 3];

                const pixel = Jimp.rgbaToInt(red, green, blue, alpha);
                const pal = alpha == 0 ? 0 : paletteArray.find((t) => { return pixel === t.pixel })
                if (!pal) {
                    throw `palette not found ${pixel}`
                }
                data.push(pal.id)
            });

            cbk(data, tiles.length, x / tileSize, y / tileSize);
            tiles.push(data)
        }
    }

    return tiles;
}

export function RGB8888To555(rgba: RGBA) {
    return `COLOR_RGB1555(${rgba.a ? 1 : 0}, ${Math.floor(rgba.r / 8)}, ${Math.floor(rgba.g / 8)}, ${Math.floor(rgba.b / 8)})`
}
