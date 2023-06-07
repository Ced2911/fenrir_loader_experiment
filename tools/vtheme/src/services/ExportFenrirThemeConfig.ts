import { type Area, type BackgroundSetting, type BrowserItemColor, type BrowserSetting, type CoverSetting, type FenrirConfig, type RgbColor } from "@/models/screens";

import { DVBuffer, RGBFunc } from '@/services/Utils'
import { Buffer } from "buffer";

const THEME_VERSION = 0x00000000
const F16_1 = 0x00010000

export enum THEME_ID {
    THEME_CONFIG_V0 = 0x000000001,
    VDP2_BG = 0x000000010,
    VDP2_FG = 0x000000011,
    FONT = 0x000000100,
    VGM_SN76xxx = 0x000001000,
};

export function ThemeConfigToBuffer(config: FenrirConfig) {
    const buffer = new DVBuffer()

    function _RgbColor(c: RgbColor) {
        buffer.addUint16(RGBFunc.getRgb555(c))
    }

    function _Area(area: Area) {
        buffer.addUint16(area.x)
        buffer.addUint16(area.y)
        buffer.addUint16(area.w)
        buffer.addUint16(area.h)
    }

    function _BrowserItemColor(c: BrowserItemColor) {
        // main color
        _RgbColor(c.main_colors.color)
        _RgbColor(c.main_colors.shadow)

        // gradient
        _RgbColor(c.gradient_colors.tl)
        _RgbColor(c.gradient_colors.tr)
        _RgbColor(c.gradient_colors.br)
        _RgbColor(c.gradient_colors.bl)
    }

    function _BrowserSetting(s: BrowserSetting) {
        _Area(s)
        buffer.addUint16(s.line_height)
        _BrowserItemColor(s.item_color)
        _BrowserItemColor(s.focused_color)
    }

    function _CoverSetting(c: CoverSetting) {
        _Area(c)
        buffer.addUint16(c.enabled ? 1 : 0)
    }

    function _BackgroundSetting(b: BackgroundSetting) {

        buffer.addUint32(b.x_inc * F16_1)
        buffer.addUint32(b.y_inc * F16_1)
    }


    function _FenrirConfig(cfg: FenrirConfig) {
        // gamelist
        _BrowserSetting(cfg.screens.gamelist.browser)
        _CoverSetting(cfg.screens.gamelist.cover)
        _BackgroundSetting(cfg.screens.gamelist.backgound)
    }
    _FenrirConfig(config)

    return buffer
}


export class ThemeExport {
    ressources: Record<string, Uint8Array> = {}

    addRessource(id: THEME_ID, dv: DVBuffer | Buffer) {
        if (dv instanceof DVBuffer) {
            // align to 0x100
            this.ressources[`${id}`] = dv.data(0x100)
        }
        else {
            const sz = (dv.length + 0x100) & ~0x100
            // align to 0x100
            this.ressources[`${id}`] = (new Uint8Array(dv, 0, sz))
        }

    }

    build() {
        const headerDV = new DVBuffer()
        let archiveSz = 0x100

        // build header
        const ressource_table_pos = 0x80;
        let ressource_offset = 0x100
        Object.entries(this.ressources).forEach(([id, buf], n) => {
            const offset = (n * 0x10);
            const iidx = parseInt(id)

            // id + offset + size
            headerDV.setUint32(ressource_table_pos + offset + 0, iidx)
            headerDV.setUint32(ressource_table_pos + offset + 4, ressource_offset)
            headerDV.setUint32(ressource_table_pos + offset + 8, buf.length)

            ressource_offset += buf.length

            archiveSz += buf.length
        })

        headerDV.setUint32(0, THEME_VERSION)
        headerDV.setUint32(4, archiveSz)
        headerDV.setUint32(8, Object.values(this.ressources).length)
        headerDV.seek(0x100);

        return Buffer.concat([headerDV.data(0), ...Object.values(this.ressources)])

    }

}
