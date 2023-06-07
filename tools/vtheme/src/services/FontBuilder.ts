import { DVBuffer } from "./Utils"

export const fonts = [
    {
        url: "https://fonts.cdnfonts.com/s/71909/PublicPixel-eZPz6.woff",
        name: 'public-pixel',
        size: 8,
        width: 8,
        height: 8,
    },
    {
        url: "https://fonts.cdnfonts.com/s/67445/Quinquefive-Ea6d4.woff",
        name: 'quinquefive',
        size: 5,
        width: 5,
        height: 5,
    }

]

export class FontBuilder {
    canvas: HTMLCanvasElement
    ctx2d: CanvasRenderingContext2D
    chars: string[]
    font: string = ""
    curFont: any
    fontInfo: Record<string, { width: number }> = {}

    constructor(canvas: HTMLCanvasElement) {
        this.canvas = canvas
        // @ts-ignore
        this.ctx2d = canvas.getContext("2d")
        this.chars = Array.from({ length: 256 - 32 }, (x, idx) => String.fromCharCode(32 + idx))

        // default font
        //this.setFont('8px "public-pixel"')
    }

    async loadAllFonts() {
        const ps = fonts.map(async font => {
            return new Promise((resolve, rej) => {
                console.log(font)
                const ff = new FontFace(font.name, `url(${font.url})`);
                ff.load().then((f) => {
                    document.fonts.add(f)
                    resolve(f)
                }).catch(rej)
            })
        })
        return await Promise.all(ps)
    }

    _getFont(_name: string) {
        return fonts.find(({ name }) => name == _name)
    }

    setFont(f: typeof fonts[number]['name']) {

        const font = this._getFont(f)
        if (font) {
            this._setFont(`${font.size}px "${font.name}"`)
            this.curFont = font
        }
    }

    _setFont(f: string) {
        this.font = f
    }

    _buildFontInfo() {
        const mt = this.ctx2d.measureText('a')
        console.log('a', this.ctx2d.measureText('a'))
        console.log('A', this.ctx2d.measureText('A'))
        console.log('.', this.ctx2d.measureText('.'))
        console.log('|', this.ctx2d.measureText('|'))

        this.chars.forEach(c => {
            const mt = this.ctx2d.measureText(c)
            this.fontInfo[c] = { width: mt.width }
        })

        console.log(this.fontInfo)
    }

    drawCharInCanvas() {
        this.ctx2d.font = this.font
        this.ctx2d.filter = 'url(#remove-alpha)';
        // @ts-ignore
        this.ctx2d.textRendering = "geometricPrecision"
        this.ctx2d.textBaseline = 'top'
        this.ctx2d.fillStyle = "#ff00ff"
        this.ctx2d.fontKerning = "none"
        this.chars.forEach((c, id) => {
            this.ctx2d.fillText(c, id * this.curFont.width, 0)
        })
        console.log(this.ctx2d, "draw")


        this._buildFontInfo()
    }


    _U8toU4(u8: Uint8ClampedArray) {
        // u8 => RGBA32
        const u4 = new Uint8Array(u8.length / 8)
        for (let i = 0, j = 0; i < u8.length; i += 8, j++) {
            const p0 = (u8[i] & 1) & 0xf
            const p1 = (u8[i + 4] & 1) & 0xf
            u4[j] = p1 | (p0 << 4);
        }
        return u4
    }

    buildFont() {
        const OFF_CHAR_W = 0
        const OFF_CHAR_H = 4
        const OFF_CHAR_SPACE = 8
        const OFF_CHAR_PIXELS = OFF_CHAR_SPACE + 256

        const buff = new DVBuffer()

        // font info
        for (let i = 0; i < 256; i++) {
            let width = 8;
            buff.setUint8(OFF_CHAR_SPACE + i, width)
        }

        // add info
        // char w
        buff.setUint32(OFF_CHAR_W, this.curFont.size)
        // char h
        buff.setUint32(OFF_CHAR_H, this.curFont.size)

        // pixels
        this.chars.forEach((c, id) => {
            const imgData = this.ctx2d.getImageData(id * this.curFont.width, 0, this.curFont.width, this.curFont.height)
            const u4 = this._U8toU4(imgData.data)

            buff.seek(
                OFF_CHAR_PIXELS +
                (id * this.curFont.width * this.curFont.height) / 2)

            //console.log(imgData.data, u4)
            for (let i = 0; i < u4.length; i++) {
                buff.addUint8(u4[i])
            }
        })

        return buff.data()

    }
} 