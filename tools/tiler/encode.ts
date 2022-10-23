
import { RGBA, Cell, RGB8888To555Number } from './tiler'
interface Palettes {
    palettes: RGBA[]
};

const b16 = 1;
// see /opt/tool-chains/sh2eb-elf/sh2eb-elf/include/yaul/scu/bus/b/vdp/vdp2/scrn_macros.h

/*
#define VDP2_SCRN_PND_CONFIG_0(cram_mode, cpd_addr, pal_addr, vf, hf)          \
        (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |       \
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFF))
         */
function encodePatternU16(addr: number, vf: boolean, hf: boolean) {
    return (addr >> 5) | (vf ? 1 : 0) << 11 | (hf ? 1 : 0) << 10;
}

/*
#define VDP2_SCRN_PND_CONFIG_8(cram_mode, cpd_addr, pal_addr, vf, hf, pr, cc)  \
        ((((vf) & 0x01) << 31) |                                               \
         (((hf) & 0x01) << 30) |                                               \
         (((pr) & 0x01) << 29) |                                               \
         (((cc) & 0x01) << 28) |                                               \
         ((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x007F) << 16) |       \
         (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x7FFF))
         */
function encodePatternU32(addr: number, vf: boolean, hf: boolean) {
    return (addr >> 5) | (vf ? 1 : 0) << 31 | (hf ? 1 : 0) << 30;
}

export function patternGetPage(x: number, y: number) {

    const page_sz = 63
    const page = x > page_sz ? 1 : 0 + y > page_sz ? 2 : 0;
    return page
}

export function cellPattern(cell: any) {
    const encodePattern = b16 ? encodePatternU16 : encodePatternU32

    const ptn = cell.id

    const vf = cell.mirror & 1;
    const hf = (cell.mirror >> 1) & 1;

    const addr = ptn * 8 * 8;
    const ptn_ss = encodePattern(addr, vf, hf);
    return ptn_ss;
}

export function exportPaletteToBin(pal: Palettes): Buffer {
    const bin = Buffer.alloc(pal.palettes.length * 2);
    const u16 = Uint16Array.from(pal.palettes.map(c => RGB8888To555Number(c)));
    u16.forEach((v, k) => {
        bin.writeUInt16BE(v, k << 1)
    })
    return bin;
}

export function exporPtnToBin(pattern: number[]): Buffer {
    if (b16) {
        const bin = Buffer.alloc(pattern.length * 2);
        const u16 = Uint16Array.from(pattern);
        u16.forEach((v, k) => {
            bin.writeUInt16BE(v, k << 1)
        })
        return bin;

    } else {
        const bin = Buffer.alloc(pattern.length * 4);
        const u32 = Uint16Array.from(pattern);
        u32.forEach((v, k) => {
            bin.writeUInt32BE(v, k << 1)
        })
        return bin;
    }
}

export function exportCellsToBin(cells: Cell[]): Buffer {
    const d = Uint8Array.from(cells.flatMap(cell => cell.data))
    //console.log(cells)
    return Buffer.from(d.buffer);
}

export function exportBufferToC(key: string, buf: Buffer): string {
    const arHex = Array.from(buf).map(v => `0x` + v.toString(16))
    const str = `
static const size_t ${key}_sz = ${buf.length};
static const uint8_t ${key}[] = {
    ${arHex.join(',')}
};`
        ;
    return str;
}
