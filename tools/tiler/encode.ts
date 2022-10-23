
import { RGBA, Cell, RGB8888To555Number } from './tiler'
interface Palettes {
    palettes: RGBA[]
};

export function exportPaletteToBin(pal: Palettes): Buffer {
    const bin = Buffer.alloc(pal.palettes.length * 2);
    const u16 = Uint16Array.from(pal.palettes.map(c => RGB8888To555Number(c)));
    u16.forEach((v, k) => {
        bin.writeUInt16BE(v, k << 1)
    })
    return bin;
}
export function exporPtnToBin(pattern: number[]): Buffer {
    const bin = Buffer.alloc(pattern.length * 2);
    const u16 = Uint16Array.from(pattern);
    u16.forEach((v, k) => {
        bin.writeUInt16BE(v, k << 1)
    })
    return bin;
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
