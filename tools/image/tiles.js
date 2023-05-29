const Jimp = require('jimp');
const { createHash } = require('crypto');
const { fstat, writeFileSync } = require('fs');

function hash(tile) {
    return createHash('sha256').update(tile.bitmap.data).digest('hex');
}

function RGB8888To555Number(a, b, g, r) {
    return (a ? 0x8000 : 0) |
        ((b >> 3) << 10) |
        ((g >> 3) << 5) |
        ((r >> 3));
}

class DVBuffer {
    constructor() {
        this.ar = new ArrayBuffer(1024 * 1024 * 30)
        this.dv = new DataView(this.ar)
        this.offset = 0

    }
    addUint16(v) {
        this.dv.setUint16(this.offset, v, false)
        this.offset += 2
    }
    getUint16(off) {
        return this.dv.getUint16(off * 2)
    }
    addUint8(v) {
        this.dv.setUint8(this.offset, v)
        this.offset++
    }
    seek(pos) {
        this.offset = pos
    }
    getUint8(off) {
        return this.dv.getUint8(off)
    }
    count() {
        return this.offset
    }
    data() {
        return Buffer.from(this.ar.slice(0, this.offset))
    }
}

(async () => {

    const vdp_config = {
        map_sz: 1,
        plane: 1,
        page: 64,
        tile: 8, // 8 or 16,
        bpp: 4
    }

    const hashm = {
    }

    const mapData = new DVBuffer()
    const tileData = new DVBuffer()
    const palData = new DVBuffer()


    const tiles = []
    const unik_tiles = []
    const palettes = []
    const colors = []

    const image = await Jimp.read("/workspaces/loader_yaul/assets/map.png")

    // extract the palettes
    image.scan(0, 0, image.bitmap.width, image.bitmap.height, function (x, y, idx) {
        const color = RGB8888To555Number(this.bitmap.data[idx + 3], this.bitmap.data[idx + 2], this.bitmap.data[idx + 1], this.bitmap.data[idx + 0])
        colors.push(color)
    })

    palettes.push(0, ... new Set(colors))
    const palette_flip = Array.from(palettes.keys()).reduce(function (obj, v) {
        obj[palettes[v]] = v; return obj
    }, {})

    palettes.forEach(c => palData.addUint16(c))

    // tiling

    for (let map_y = 0; map_y < vdp_config.map_sz; map_y++) {
        for (let map_x = 0; map_x < vdp_config.map_sz; map_x++) {
            for (let ply = 0; ply < vdp_config.plane; ply++) {
                for (let plx = 0; plx < vdp_config.plane; plx++) {
                    for (let y = 0; y < vdp_config.page; y++) {
                        for (let x = 0; x < vdp_config.page; x++) {


                            const plane_x_stride = (map_x * vdp_config.plane * vdp_config.page * vdp_config.tile)
                            const plane_y_stride = (map_y * vdp_config.plane * vdp_config.page * vdp_config.tile)

                            const page_x_stride = (plx * vdp_config.page * vdp_config.tile)
                            const page_y_stride = (ply * vdp_config.page * vdp_config.tile)

                            const tile_x_stride = (x * vdp_config.tile)
                            const tile_y_stride = (y * vdp_config.tile)

                            const pos_x = plane_x_stride + page_x_stride + tile_x_stride
                            const pos_y = plane_y_stride + page_y_stride + tile_y_stride


                            const tile = new Jimp(vdp_config.tile, vdp_config.tile)

                            tile.blit(image, 0, 0, pos_x, pos_y, vdp_config.tile, vdp_config.tile);


                            // js is too slow with image manipulations
                            // compute hash

                            tiles.push({
                                tile: tile,
                                hash: hash(tile),
                                hash_x: hash(await tile.clone().flip(true, false)),
                                hash_y: hash(await tile.clone().flip(false, true)),
                                hash_xy: hash(await tile.clone().flip(true, true)),
                            })
                        }
                    }
                }
            }
        }
    }

    tiles.forEach(tile => {
        let add = true

        unik_tiles.every(ut => {
            const flip_n = ut.hash == tile.hash
            const flip_x = ut.hash_x == tile.hash
            const flip_y = ut.hash_y == tile.hash
            const flip_xy = ut.hash_xy == tile.hash

            if ((flip_n || flip_x || flip_y || flip_xy))
                add = false

            return add
        })

        if (add)
            unik_tiles.push(tile)
    })

    unik_tiles.forEach((t, idx) => { t.tile.write(`tmp/u_${idx}.png`); })

    unik_tiles.forEach((ut, id) => {
        hashm[ut.hash] = (0 << 10) | (id << 0);
        hashm[ut.hash_x] = (1 << 10) | (id << 0);
        hashm[ut.hash_y] = (2 << 10) | (id << 0);
        hashm[ut.hash_xy] = (3 << 10) | (id << 0);
    })

    // create map
    tiles.forEach(tile => {
        const attr = hashm[tile.hash]
        if (attr == undefined) {
            throw "err"
        }
        mapData.addUint16(attr)
    })

    // rebuild image :D
    if (true) {
        const dbg = new Jimp(512, 512)

        for (let i = 0; i < mapData.count() / 2; i++) {
            let p = i
            const pos_x = (p * vdp_config.tile) % 512
            const pos_y = Math.trunc((p * vdp_config.tile) / 512) * vdp_config.tile

            const id = (mapData.getUint16(i) & 0x3ff) >> 0
            const tile = unik_tiles[id].tile
            await dbg.blit(tile, pos_x, pos_y);
        }

        dbg.writeAsync("tmp/dbg.png")
    }


    // tiles to palette
    unik_tiles.forEach(({ tile }) => {
        if (1) {
            const uint4b = []
            tile.scan(0, 0, tile.bitmap.width, tile.bitmap.height, function (x, y, idx) {
                const color = RGB8888To555Number(this.bitmap.data[idx + 3], this.bitmap.data[idx + 2], this.bitmap.data[idx + 1], this.bitmap.data[idx + 0])
                uint4b.push(palette_flip[color])
            })

            for (let i = 0; i < uint4b.length; i += 2) {
                tileData.addUint8((uint4b[i + 0] << 4) | uint4b[i + 1])
            }
        } else {
            tile.scan(0, 0, tile.bitmap.width, tile.bitmap.height, function (x, y, idx) {
                const color = RGB8888To555Number(this.bitmap.data[idx + 3], this.bitmap.data[idx + 2], this.bitmap.data[idx + 1], this.bitmap.data[idx + 0])
                tileData.addUint8(palette_flip[color])
            })
        }
    })

    const header = new DVBuffer()

    // write header
    header.addUint8(vdp_config.map_sz)
    header.addUint8(vdp_config.plane)
    header.addUint8(vdp_config.page)
    header.addUint8(vdp_config.tile)

    header.addUint8(vdp_config.bpp)
    header.addUint8(0) /* pad */
    header.addUint16(unik_tiles.length)


    header.seek(0x80)
    palData.seek(0x80)


    const towrite = Buffer.concat([
        header.data(),
        palData.data(),
        mapData.data(),
        tileData.data()
    ])

    writeFileSync('vdp2.dat', towrite)

})()