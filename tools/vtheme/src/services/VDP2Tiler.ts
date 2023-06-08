import * as JimpType from 'jimp/types';
import { Buffer } from 'buffer'
import { DVBuffer } from '@/services/Utils'

interface VDP2TilerConfig {
    map_sz: 1 | 2,
    plane: 1 | 2,
    page: 32 | 64,
    tile: 8 | 16,
    bpp: 4 | 8
}

interface TileHash {
    hash: number,
    hash_x: number,
    hash_y: number,
    hash_xy: number,
    tile: JimpType,
}

function hashCode(buf: any) {
    let hash = 0;
    for (let i = 0, len = buf.length; i < len; i++) {
        const c = buf[i];
        hash = (hash << 5) - hash + c;
        hash |= 0; // Convert to 32bit integer
    }
    return hash;
}

function hash(tile: JimpType) {
    //return createHash('sha256').update(tile.bitmap.data).digest('hex');
    return hashCode(tile.bitmap.data)
}

function RGB8888To555Number(a: number, b: number, g: number, r: number) {
    if (a)
        return 0x8000 |
            ((b >> 3) << 10) |
            ((g >> 3) << 5) |
            ((r >> 3));
    else
        return 0
}


export default async function ImageTiler(i: any) {

    const vdp_config = {
        map_sz: 1,
        plane: 1,
        page: 64,
        tile: 8, // 8 or 16,
        bpp: 4
    }

    const hashm: Record<number, number> = {
    }

    const mapData = new DVBuffer()
    const tileData = new DVBuffer()
    const palData = new DVBuffer()


    const tiles: TileHash[] = []
    const unik_tiles: TileHash[] = []
    const palettes: number[] = []
    const colors: number[] = []

    //const image = await Jimp.read("/workspaces/loader_yaul/assets/aurelie.png")
    const image: JimpType = await Jimp.read(i)

    // always add transparent first
    colors.push(0)

    // extract the palettes
    image.scan(0, 0, image.bitmap.width, image.bitmap.height, function (x, y, idx) {
        const color = RGB8888To555Number(this.bitmap.data[idx + 3], this.bitmap.data[idx + 2], this.bitmap.data[idx + 1], this.bitmap.data[idx + 0])
        colors.push(color)
    })

    palettes.push(...new Set(colors))
    const palette_flip = Array.from(palettes.keys()).reduce(function (obj: Record<number, number>, v) {
        obj[palettes[v]] = v;
        return obj
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

    // unik_tiles.forEach((t, idx) => { t.tile.write(`tmp/u_${idx}.png`); })

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


    // tiles to palette
    unik_tiles.forEach(({ tile }) => {
        const uint4b: number[] = []
        tile.scan(0, 0, tile.bitmap.width, tile.bitmap.height, function (x, y, idx) {
            const color = RGB8888To555Number(this.bitmap.data[idx + 3], this.bitmap.data[idx + 2], this.bitmap.data[idx + 1], this.bitmap.data[idx + 0])
            uint4b.push(palette_flip[color])
        })

        for (let i = 0; i < uint4b.length; i += 2) {
            tileData.addUint8((uint4b[i + 0] << 4) | uint4b[i + 1])
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

    // writeFileSync('vdp2.dat', towrite)

    return towrite
}