import { RGBA, Cell, RGB8888To555Number, Cell8bppTo4bpp } from './tiler'

import LZ4 from 'lz4'; import cstruct, { Struct } from 'struct';


interface Palettes {
    palettes: RGBA[]
};

export enum Vdp2Screen { nbg0 = 'nbg0', nbg2 = 'nbg2', nbg3 = 'nbg3', nbg1 = 'nbg1', }
export enum Vdp2ColorCnt { color4bpp, color8bpp }

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
    return (addr >> 5) | ((vf ? 1 : 0) << 31) | ((hf ? 1 : 0) << 30);
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

export function cell4pbbPattern(cell: any) {
    const encodePattern = b16 ? encodePatternU16 : encodePatternU32

    const ptn = cell.id

    const vf = cell.mirror & 1;
    const hf = (cell.mirror >> 1) & 1;

    const addr = ptn * 4 * 8;
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

export function exportCellsToBin(cells: number[]): Buffer {
    const d = Uint8Array.from(cells)
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

interface VdpCycpTiming {
    cell: Vdp2Screen[],
    pattern: Vdp2Screen[]
}

export class VDP2Memory {
    vmem: Buffer
    cmem: Buffer
    size: number

    sharedCells: { mem?: Buffer, color: Vdp2ColorCnt }
    cells: Partial<Record<Vdp2Screen, { mem?: Buffer, addr: number, color: Vdp2ColorCnt }>>
    patterns: Partial<Record<Vdp2Screen, { mem?: Buffer, addr: number }>>
    palettes: Partial<Record<Vdp2Screen, { mem?: Buffer, addr: number }>>

    vdp2_cycp: VdpCycpTiming[]

    palSz: number

    struct: Struct

    constructor() {
        this.size = 0;
        this.vmem = Buffer.alloc(512 * 1024)
        this.cmem = Buffer.alloc(4 * 1024)
        this.sharedCells = { color: Vdp2ColorCnt.color8bpp }
        this.cells = {}
        this.patterns = {}
        this.palettes = {}

        this.palSz = Vdp2ColorCnt.color4bpp
        Object.values(Vdp2Screen).forEach(scr => {
            this.cells[scr] = { addr: 0, color: this.palSz }
            this.patterns[scr] = { addr: 0 }
            this.palettes[scr] = { addr: 0 }
        })

        this.vdp2_cycp = [{
            cell: [],
            pattern: []
        }, {
            cell: [],
            pattern: []
        }, {
            cell: [],
            pattern: []
        }, {
            cell: [],
            pattern: []
        }]

        this.struct = cstruct()/*header start*/
        .word8('version')
        .word32Ube('vdp2mem_sz') 
        .word32Ube('vdp2pal_sz')
        .word32Ube('vdp2mem_off')
        .word32Ube('vdp2pal_off')
        
        /*header end*/
    }

    addSharedCells(cells: number[], colorcnt: Vdp2ColorCnt) {
        const cellbpp = colorcnt == Vdp2ColorCnt.color4bpp ? Cell8bppTo4bpp(cells) : cells
        const bin = exportCellsToBin(cellbpp);
        this.sharedCells.mem = bin
        this.sharedCells.color = colorcnt

        this.cells[Vdp2Screen.nbg0].color = colorcnt
        this.cells[Vdp2Screen.nbg2].color = colorcnt
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
    exportToCompressed(): string {
        this.fillmem()
        let exp = ''

        const vmemz = Buffer.alloc(LZ4.encodeBound(this.size))
        const cmemz = Buffer.alloc(LZ4.encodeBound(this.cmem.length))
        const vmem_sz = LZ4.encodeBlock(this.vmem.slice(0, this.size), vmemz)
        const cmem_sz = LZ4.encodeBlock(this.cmem, cmemz)

        /*
         const vmemz = LZ4.encode(this.vmem.slice(0, this.size), { dict:true, highCompression: true, streamChecksum: false })
         const cmemz = LZ4.encode(this.cmem, { dict:true, highCompression: true, streamChecksum: false })
         const vmem_sz = vmemz.length
         const cmem_sz = cmemz.length
         */

        console.log('vmemz', vmem_sz)
        console.log('cmemz', cmem_sz)

        exp += exportBufferToC("vmemz", vmemz.slice(0, vmem_sz));
        exp += exportBufferToC("cmemz", cmemz.slice(0, cmem_sz));

        return exp;
    }

    exportCyclePattern(): string {
        let str = `
static inline void __setup_vdp2_cycles() {
    vdp2_vram_cycp_t vram_cycp;`;
        this.vdp2_cycp.forEach((c, idx) => {
            const access = new Array(8).fill('VDP2_VRAM_CYCP_NO_ACCESS')
            let n = 0;
            c.cell.forEach(scr => {
                if (this.cells[scr].color == Vdp2ColorCnt.color8bpp)
                    access[n++] = `VDP2_VRAM_CYCP_CHPNDR_${scr.toLocaleUpperCase()}`
                access[n++] = `VDP2_VRAM_CYCP_CHPNDR_${scr.toLocaleUpperCase()}`
            })
            c.pattern.forEach(scr => {
                access[n++] = `VDP2_VRAM_CYCP_PNDR_${scr.toLocaleUpperCase()}`
                //access[n++] = `VDP2_VRAM_CYCP_PNDR_${scr.toLocaleUpperCase()}`
            })
            str += '\n\n' + access.map((acc, jdx) => {
                return `\tvram_cycp.pt[${idx}].t${jdx} = ${acc};`
            }).join('\n')
        })

        str += `    
    vdp2_vram_cycp_set(&vram_cycp);
}`
        return str;
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
        const str = Object.keys(Vdp2Screen).map(scr => {
            return __(scr, this.cells[scr].addr, this.patterns[scr].addr, this.palettes[scr].addr)
        }).join('\n');
        return str;
    }

    memoryFiled = false

    private fillmem() {
        const pall_pank_sz = 0x100;
        let palAddr = pall_pank_sz;

        if (this.memoryFiled)
            return
        this.memoryFiled = true

        // skip a0
        let currAddr = 0 << 17;

        if (this.sharedCells.mem) {
            this.sharedCells.mem.copy(this.vmem, currAddr);
            // fill pattern cycles
            this.vdp2_cycp[currAddr >> 17].cell = [Vdp2Screen.nbg0, Vdp2Screen.nbg2]

            currAddr = vdpNextPatternAddrBoundary(this.sharedCells.mem.length);
            if (currAddr >> 17 == 0) {
                //    currAddr = 1 << 17
            }
        }

        Object.entries(this.patterns).forEach(([scr, pat]) => {
            if (pat.mem) {
                pat.addr = currAddr;
                pat.mem.copy(this.vmem, currAddr);

                // fill pattern cycles
                this.vdp2_cycp[currAddr >> 17].pattern.push(scr as Vdp2Screen)

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
        // if ng0, nbg1, nbg2 are on same bank, move nbg1 in next bank


        const currBank = currAddr >> 17
        if (
            currBank == (this.patterns[Vdp2Screen.nbg0].addr >> 17) &&
            currBank == (this.patterns[Vdp2Screen.nbg2].addr >> 17)) {
        } {
            //move to next bank
            //currAddr = ((currAddr >> 17) + 1) << 17
        }
        this.vdp2_cycp[currAddr >> 17].pattern.push(Vdp2Screen.nbg1)
        this.vdp2_cycp[(currAddr + 0x04000) >> 17].cell.push(Vdp2Screen.nbg1)

        this.cells[Vdp2Screen.nbg1].addr = currAddr + 0x04000;
        this.cells[Vdp2Screen.nbg1].color = Vdp2ColorCnt.color4bpp
        this.patterns[Vdp2Screen.nbg1].addr = currAddr;
        this.palettes[Vdp2Screen.nbg1].addr = palAddr;
    }
}