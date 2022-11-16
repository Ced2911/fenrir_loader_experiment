import { VDP2Memory, Vdp2Screen } from './encode'
import { Config, tileVdp2, VDP2ScreenConfig } from './vdp2tiler'

export interface VDP2RessourceConfig {
    screens: string[],
    type: string,
    file: string
}

export interface RessourceConfig {
    vdp2: VDP2RessourceConfig[],
    font: string
}

export interface FontRessource {
    data: number[],
    char_spacing: number[],
    char_height: number,
    char_width: number
}

export class RessourceImporter {
    vdp2mem = new VDP2Memory
    vdp2config: VDP2ScreenConfig = {}

    async importVdp2Ressources(config: VDP2RessourceConfig[]) {

        const map2bg = { "background": Vdp2Screen.nbg0, "foreground": Vdp2Screen.nbg2 }
        const tilerConfig: Config = { images: [], output: "" }

        config.forEach((cfg, k) => {
            tilerConfig.images.push({
                "key": [k, cfg.type, map2bg[cfg.type], ...cfg.screens].join('-'),
                "screen": map2bg[cfg.type],
                "file": cfg.file
            })
        })

        await tileVdp2(tilerConfig, this.vdp2mem, this.vdp2config)
        this.vdp2mem.fillmem()
    }

    async importRessources(config:RessourceConfig) {
        await this.importVdp2Ressources(config.vdp2)
    }
}
