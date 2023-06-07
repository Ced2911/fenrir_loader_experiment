import { fenrirDefaultConfig, type FenrirConfig } from '@/models/screens'
import { defineStore } from 'pinia'
import { useStorage, type RemovableRef } from '@vueuse/core'

export interface State {
    config: RemovableRef<FenrirConfig>,
    backgroundImage: RemovableRef<string>, /* base 64*/
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
            backgroundImage: useStorage<string>('theme-bg', '', undefined, {
                serializer: b64serializer
            }),

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
    }
})
