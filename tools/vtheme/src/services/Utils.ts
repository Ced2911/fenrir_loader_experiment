import type { RgbColor } from "@/models/screens"
import { Buffer } from "buffer"

const MAX_BUFFER_SIZE = (1024 * 1024)

export function downloadBuffer(buffer: Buffer, name: string = "file.dat") {
    const blob = new Blob([buffer])
    const anchor = document.createElement('a')
    anchor.href = URL.createObjectURL(blob)
    anchor.download = name
    document.body.appendChild(anchor)
    anchor.click()
}

export function saveToLocalStorage(key: string, data: any) {
    localStorage.setItem(key, btoa(data))
}

export function getFromLocalStorage(key: string) {
    const s = localStorage.getItem(key)
    if (!s)
        return null
    return atob(s)
}


export class DVBuffer {
    ar = new ArrayBuffer(MAX_BUFFER_SIZE)
    dv = new DataView(this.ar)
    offset = 0

    // add
    addUint32(v: number) {
        this.dv.setUint32(this.offset, v, false)
        this.offset += 4
    }
    addUint16(v: number) {
        this.dv.setUint16(this.offset, v, false)
        this.offset += 2
    }
    addUint8(v: number) {
        this.dv.setUint8(this.offset, v)
        this.offset++
    }

    // set
    setUint32(off: number, v: number) {
        this.dv.setUint32(off, v, false)
    }
    setUint16(off: number, v: number) {
        this.dv.setUint16(off, v, false)
    }
    setUint8(off: number, v: number) {
        this.dv.setUint8(off, v)
    }

    // get
    getUint32(off: number) {
        return this.dv.getUint32(off * 4)
    }
    getUint16(off: number) {
        return this.dv.getUint16(off * 2)
    }
    getUint8(off: number) {
        return this.dv.getUint8(off)
    }

    // 
    seek(pos: number) {
        this.offset = pos
    }
    count() {
        return this.offset
    }
    data(align?: number) {
        let sz = this.offset
        if (align)
            sz = ((align - 1) & sz) ? ((sz + align) & ~(align - 1)) : sz;


        return Buffer.from(this.ar.slice(0, sz))
    }
}


export const RGBFunc = {
    getRgb555(rgb32: number) {
        const r = (rgb32 >> (0 + 3)) & 0x1f
        const g = (rgb32 >> (8 + 3)) & 0x1f
        const b = (rgb32 >> (16 + 3)) & 0x1f
        return 0x8000 | r << 10 | g << 5 | b
    },
    fromHexString(string: string): RgbColor {

        return parseInt(string.replace('#', ''), 16)
    },

    toHexString(c: Number | RgbColor | undefined) {
        if (c !== undefined && Number.isInteger(c))
            return `#${c.toString(16).padStart(6, '0')}`
        else {
            return `#${(0).toString(16).padStart(6, '0')}`
        }
    },
    multiplyColor(color: number, factor: number) {
        function getRGB(c: number) {
            const r = (c >> (16)) & 0xff
            const g = (c >> (8)) & 0xff
            const b = (c >> (0)) & 0xff

            return { r, g, b }
        }
        function m(a: number, b: number) {
            const fa = a / 255
            const fb = (((b / 255) * 2) - 1) / 2
            const r = (fa + fb)
            const v = Math.round(r * 255)
            return Math.min(Math.max(v, 0), 255)
        }

        const cc = getRGB(color)
        const cfactor = getRGB(factor)

        const out = ((m(cc.r, cfactor.r) << 16) | (m(cc.g, cfactor.g) << 8) | (m(cc.b, cfactor.b)))
        return out
    }
}




export async function BlobToBase64(blob: Blob) {
    const imgB64 = await new Promise((resolve, _) => {
        const reader = new FileReader()
        reader.onloadend = () => resolve(reader.result)
        reader.readAsDataURL(blob)
    })

    if (imgB64) {
        // @ts-ignore
        const d = imgB64.split(',')
        return atob(d[1])
    }

}


export function SetCssVar(varn: string, value: string) {
    document.documentElement.style.setProperty(
        varn,
        value
    )
}