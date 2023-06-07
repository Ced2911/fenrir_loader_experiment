import { fenrirDefaultConfig, type FenrirConfig } from '@/models/screens'
import { defineStore } from 'pinia'
import { useStorage, type RemovableRef } from '@vueuse/core'
import ImageTiler from '@/services/VDP2Tiler'
import { THEME_ID, ThemeConfigToBuffer, ThemeExport } from '@/services/ExportFenrirThemeConfig'
import { DVBuffer, downloadBuffer } from '@/services/Utils'
import { fonts, FontBuilder } from '@/services/FontBuilder'

export interface State {
    config: RemovableRef<FenrirConfig>,
    backgroundImage: RemovableRef<string>, /* base 64*/
    font: RemovableRef<typeof fonts[number]['name']>,
}


const context: { fontCanvas?: HTMLCanvasElement, fontBuilder?: FontBuilder } = {
    fontCanvas: undefined,
    fontBuilder: undefined
}

const b64serializer = {
    read: (v: string) => {
        // base 64 to blob
        const b = atob(v)
        return b
    },
    write: (b64: string) => {
        return btoa(b64)
    }
}

export const useThemeConfigStore = defineStore('theme', {
    state: (): State => {
        return {
            config: useStorage<FenrirConfig>('theme-config', fenrirDefaultConfig),
            backgroundImage: useStorage<string>('theme-background', '', undefined, {
                serializer: b64serializer
            }),
            font: useStorage<string>('theme-font', fonts[0].name),
        }
    },
    getters: {
        backgroundImageUrl(state) {
            const ii = Uint8Array.from(state.backgroundImage, (c) => c.charCodeAt(0))
            const blobUrl = URL.createObjectURL(new Blob([ii]))
            return blobUrl
        }
    },

    actions: {
        async updateBackground(blob: Blob) {
            const imgB64 = await new Promise((resolve, _) => {
                const reader = new FileReader()
                reader.onloadend = () => resolve(reader.result)
                reader.readAsDataURL(blob)
            })

            if (imgB64) {
                // @ts-ignore
                const d = imgB64.split(',')
                this.backgroundImage = atob(d[1])
            }
        },
        async initFonts(canvas: HTMLCanvasElement) {
            context.fontBuilder = new FontBuilder(canvas);
            context.fontCanvas = canvas

            await context.fontBuilder.loadAllFonts()

            context.fontBuilder.setFont(this.font)
            context.fontBuilder.drawCharInCanvas()
        },
        async updateFont(font: string) {
            console.log('updateFont')
            this.font = font
        },
        updateGamelistFocusColors(c: any) {
            Object.assign(this.config.screens.gamelist.browser.focused_color, c)
        },
        updateGamelistItemColors(c: any) {
            Object.assign(this.config.screens.gamelist.browser.item_color, c)
        },
        updateAreaGamelistBrowser(c: any) {
            Object.assign(this.config.screens.gamelist.browser, c)
        },
        updateAreaGamelistCover(c: any) {
            Object.assign(this.config.screens.gamelist.cover, c)
        },
        async buildTheme() {
            const vdpbg = await ImageTiler(this.backgroundImageUrl)
            const th = ThemeConfigToBuffer(this.config)


            const expt = new ThemeExport()
            expt.addRessource(THEME_ID.VDP2_BG, vdpbg)
            expt.addRessource(THEME_ID.THEME_CONFIG_V0, th)

            //
            if (context.fontBuilder) {
                context.fontBuilder.setFont(this.font)
                context.fontBuilder.drawCharInCanvas()

                const fontBuffer = context.fontBuilder.buildFont()
                console.log(fontBuffer)
                expt.addRessource(THEME_ID.FONT, fontBuffer)
            }

            downloadBuffer(expt.build(), 'theme.bin')
        },


    }
})
