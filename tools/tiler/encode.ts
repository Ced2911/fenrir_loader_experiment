import { RGBA, Cell, RGB8888To555Number } from './tiler'
interface Palettes {
    palettes: RGBA[]
};

export enum Vdp2Screen { nbg0 = 'nbg0', nbg2 = 'nbg2', nbg3 = 'nbg3', nbg1 = 'nbg1', }

function nextPowerOf2(x: number) {
    return Math.pow(2, Math.ceil(Math.log(x) / Math.log(2)));
}
export function vdpNextPatternAddrBoundary(addr: number) {
    let nextAddr = nextPowerOf2(addr);
    return (nextAddr >> 12) << 12;
}


const b16 = 0;
// see /opt/tool-chains/sh2eb-elf/sh2eb-elf/include/yaul/scu/bus/b/vdp/vdp2/scrn_macros.h

/*
#define VDP2_SCRN_PND_CONFIG_0(cram_mode, cpd_addr, pal_addr, vf, hf)          \
        (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |       \
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFF))
         */
function encodePatternU16(addr: number, vf: number, hf: number) {
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
function encodePatternU32(addr: number, vf: number, hf: number) {
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
    return encodePattern(addr, vf, hf);;
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
            bin.writeUInt32BE(v, k << 2)
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



export class VDP2Memory {
    vmem: Buffer
    cmem: Buffer
    size: number

    sharedCells: { mem?: Buffer }
    cells: Partial<Record<Vdp2Screen, { mem?: Buffer, addr: number }>>
    patterns: Partial<Record<Vdp2Screen, { mem?: Buffer, addr: number }>>
    palettes: Partial<Record<Vdp2Screen, { mem?: Buffer, addr: number }>>

    constructor() {
        this.size = 0;
        this.vmem = Buffer.alloc(512 * 1024)
        this.cmem = Buffer.alloc(4 * 1024)
        this.sharedCells = {}
        this.cells = {}
        this.patterns = {}
        this.palettes = {}

        Object.values(Vdp2Screen).forEach(scr => {
            this.cells[scr] = { addr: 0 }
            this.patterns[scr] = { addr: 0 }
            this.palettes[scr] = { addr: 0 }
        })
    }

    addSharedCells(cells: Cell[]) {
        const bin = exportCellsToBin(cells);
        this.sharedCells.mem = bin
    }
    addCharPattern(screen: Vdp2Screen, pattern: number[]) {
        const bin = exporPtnToBin(pattern)
        this.patterns[screen].mem = bin
    }
    addPalette(screen: Vdp2Screen, pal: Palettes) {
        const bin = exportPaletteToBin(pal)
        this.palettes[screen].mem = bin
    }

    exportToBin(): Buffer { return this.vmem }
    exportToC(): string {
        this.fillmem()
        let exp = ''

        exp += exportBufferToC("vmem", this.vmem.slice(0, this.size));
        exp += exportBufferToC("cmem", this.cmem);
        return exp;
    }

    exportConfig(): string {
        function __(screen, cellAddr, ptnAddr, palAddr) {
            const vdp2_addr = 0x25E00000;
            const cram_addr = 0x25F00000;
            const cell = vdp2_addr + cellAddr
            const ptn = vdp2_addr + ptnAddr
            const pal = cram_addr + palAddr * 2
            return `
#define ${screen.toUpperCase()}_PATTERN_ADDR   (0x${ptn.toString(16)}UL)
#define ${screen.toUpperCase()}_CELL_ADDR      (0x${cell.toString(16)}UL)
#define ${screen.toUpperCase()}_COLOR_ADDR     (0x${pal.toString(16)}UL)
            `
        }
        function ___(screen, cellAddr, ptnAddr, palAddr) {
            return `
#define ${screen.toUpperCase()}_PATTERN_ADDR   (0x25E00000UL + 0x${ptnAddr.toString(16)})
#define ${screen.toUpperCase()}_CELL_ADDR      (0x25E00000UL + 0x${cellAddr.toString(16)})
#define ${screen.toUpperCase()}_COLOR_ADDR     VDP2_CRAM_ADDR(0x${palAddr.toString(16)})
            `
        }
        const str = Object.keys(Vdp2Screen).map(scr => {
            return __(scr, this.cells[scr].addr, this.patterns[scr].addr, this.palettes[scr].addr)
        }).join('\n');
        return str;
    }

    private fillmem() {
        const pall_pank_sz = 0x100;
        let palAddr = pall_pank_sz;
        let currAddr = 0;

        if (this.sharedCells.mem) {
            this.sharedCells.mem.copy(this.vmem);
            currAddr = vdpNextPatternAddrBoundary(this.sharedCells.mem.length);
        }

        Object.values(this.patterns).forEach((pat) => {
            if (pat.mem) {
                pat.addr = currAddr;
                pat.mem.copy(this.vmem, currAddr);


                currAddr += vdpNextPatternAddrBoundary(pat.mem.length);
            }
        })

        Object.values(this.palettes).forEach((pal) => {
            if (pal.mem) {
                pal.addr = palAddr;
                pal.mem.copy(this.cmem, palAddr * 2);

                console.log(pal.mem.length)
                palAddr += pall_pank_sz;
            }
        })

        this.size = currAddr

        // hardcoded values for ngb1 
        this.cells['nbg1'].addr = currAddr + 0x02000;
        this.patterns['nbg1'].addr = currAddr;
        this.palettes['nbg1'].addr = palAddr;
    }
}