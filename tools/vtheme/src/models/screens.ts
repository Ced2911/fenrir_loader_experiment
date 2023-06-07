import { Type } from 'class-transformer';

export class Area {
    x: number = 20;
    y: number = 18;
    w = 172;
    h = 200;
}


export type RgbColor = number

export interface BrowserItemColor {
    main_colors: { color: RgbColor, shadow: RgbColor }
    gradient_colors: { tl: RgbColor, tr: RgbColor, br: RgbColor, bl: RgbColor }
}

export interface BrowserSetting extends Area {
    x: number
    y: number
    w: number
    h: number

    line_height: number

    item_color: BrowserItemColor
    focused_color: BrowserItemColor
}

export interface CoverSetting extends Area {
    enabled: boolean
    x: number
    y: number
    w: number
    h: number
}

export interface BackgroundSetting {
    x_inc: number
    y_inc: number
}

export interface FenrirConfig {
    screens: {
        gamelist: {
            browser: BrowserSetting,
            cover: CoverSetting,
            backgound: BackgroundSetting,
            foreground: BackgroundSetting,
        }
    }
}

export const fenrirDefaultConfig = (() => {
    const config: FenrirConfig = {
        screens: {
            gamelist: {
                backgound: {
                    x_inc: 0.25,
                    y_inc: 0.25
                },
                foreground: {
                    x_inc: 0.25,
                    y_inc: 0.25
                },
                cover: {
                    x: 200,
                    y: 73,
                    w: 128,
                    h: 47,
                    enabled: true
                },
                browser: {
                    focused_color: {
                        main_colors: {
                            color: 0xff0000,
                            shadow: 0xffffff
                        },
                        gradient_colors: {
                            tl: 0x808080,
                            tr: 0x808080,
                            bl: 0x808080,
                            br: 0x808080,
                        }
                    },
                    item_color: {
                        main_colors: {
                            color: 0x880000,
                            shadow: 0x808080
                        },
                        gradient_colors: {
                            tl: 0x808080,
                            tr: 0x808080,
                            bl: 0x808080,
                            br: 0x808080,
                        }
                    },
                    x: 20,
                    y: 18,
                    w: 172,
                    h: 200,
                    line_height: 10
                }
            }
        }
    }

    return config
})()