import Jimp from 'jimp';
import { createHash } from 'crypto';
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

const palette: Record<number, RGBA> = {}
const tiles: Record<string, TILE> = {}
const map: number[] = []

function hashh(data: Buffer | number[]) {
    const hash = createHash('sha256');
    hash.update(Uint32Array.from(data))
    const hashh = hash.digest('hex')
    return hashh
}

function getImageData(image: Jimp, x: number, y: number, w: number, h: number) {
    const data: number[] = []
    image.scan(x, y, w, h, function (x, y, idx) {
        var red = this.bitmap.data[idx + 0];
        var green = this.bitmap.data[idx + 1];
        var blue = this.bitmap.data[idx + 2];
        var alpha = this.bitmap.data[idx + 3];
        var pixel = Jimp.rgbaToInt(red, green, blue, alpha);
        data.push(pixel)
    })
    return data
}


function buildPalette(image: Jimp) {
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
}

function buildTile(image: Jimp) {

    for (let y = 0; y < image.bitmap.height; y += TILE_H) {
        for (let x = 0; x < image.bitmap.width; x += TILE_W) {
            const data = getImageData(image, x, y, TILE_W, TILE_H)

            const hash = hashh(data)
            if (tiles[hash] === undefined) {
                tiles[hash] = { hash: hash, data }
            }
        }
    }
}

function buildMap(image: Jimp, tilesArray: TILE[]) {
    const TILE_W = 8;
    const TILE_H = 8;

    for (let y = 0; y < image.bitmap.height; y += TILE_H) {
        for (let x = 0; x < image.bitmap.width; x += TILE_W) {
            const data = getImageData(image, x, y, TILE_W, TILE_H)
            const hash = hashh(data)
            const tileidx = tilesArray.findIndex(t => t.hash === hash)
            map.push(tileidx)
        }
    }
}

function dbg(map: number[], tilesArray: TILE[]) {
    const image = new Jimp(512, 512, (err, image) => {
        // read map...
        map.forEach((n, idx) => {
            let xoffset = (idx % 64) * TILE_W;
            let yoffset = (Math.floor(idx / 64)) * TILE_H;
            const tiledata = tilesArray[n].data

            tiledata.forEach((palette, jdx) => {
                let x = jdx % TILE_W;
                let y = Math.floor(jdx / TILE_H);
                image.setPixelColor(palette, x + xoffset, y + yoffset)
            })
        })
        image.write('dbg.png')
    });
}

function outputTile(tilesArray: TILE[], paletteLut: Record<string, number>) {
    let tiles = tilesArray.map((t, idx) => {
        const tiledata = t.data
        const tilepal = tiledata.map(px => {
            return '0x' + paletteLut[""+px].toString(16);
        })
        return "// " + idx + "\n" + tilepal.join(', ')
    }).join(",\n")
    return tiles
}

function RGB8888To555(rgba: RGBA) {
    return `COLOR_RGB1555(${rgba.a ? 1 : 0}, ${Math.floor(rgba.r / 8)}, ${Math.floor(rgba.g / 8)}, ${Math.floor(rgba.b / 8)})`
}

Jimp.read('../../assets/bitmap.png')
    .then(image => {
        buildPalette(image)
        buildTile(image)
        const tilesArray = Object.values(tiles)
        buildMap(image, tilesArray)

        console.log(Object.values(palette).length)
        console.log(tilesArray.length)
        console.log(map.length)

        // px => arrayidx
        const paletteLut: Record<string, number> = {}
        const paletteArray: RGBA[] = []

        for (const [px, value] of Object.entries(palette)) {
            paletteLut[px] = paletteArray.length
            paletteArray.push(value)
        }


        dbg(map, tilesArray)

        let tilesstr = `
const unsigned long tilesLength = ${tilesArray.length}*sizeof(unsigned char);
const unsigned char tiles[] = {
    ${outputTile(tilesArray, paletteLut)}
};`
        let mapstr = `
const unsigned long mapLength = ${map.length}*sizeof(unsigned short);
const unsigned short map[] = {
    ${map.map(m => m).join(',')}
};`

        let palstr = `
const unsigned long palLength = ${Object.values(palette).length}*sizeof(color_rgb1555_t);
const color_rgb1555_t pal[] = {
    ${Object.values(palette).map(c => RGB8888To555(c)).join(',')}
};`
        //console.log(tilesstr)
        writeFile('../../assets/test.h', tilesstr + mapstr + palstr, () => { })
    })
    .catch(err => {
        console.error(err);
    });