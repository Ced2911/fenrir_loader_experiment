export class Area {
    x: number = 20;
    y: number = 18;
    w = 172;
    h = 200;
}

export class RgbColor {
    rgb32: number = 0

    getRgb555() {
        return this.rgb32
    }

    static fromHexString(string: string): RgbColor {
        const c = new RgbColor();
        c.rgb32 = parseInt(string.replace('#', ''), 16)
        return c
    }

    static toHexString(c: Number | RgbColor | undefined) {
        if (c === undefined) {
            return `#${(0).toString(16).padStart(6, '0')}`
        }
        else if (c instanceof RgbColor)
            return `#${c.rgb32.toString(16).padStart(6, '0')}`
        else
            return `#${c.toString(16).padStart(6, '0')}`
    }
}

export class BrowserItemColor {
    main_colors = { color: new RgbColor(), shadow: new RgbColor() }
    gradient_colors = { tl: new RgbColor(), tr: new RgbColor(), br: new RgbColor(), bl: new RgbColor() }
}

export class BrowserSetting extends Area {
    x: number = 20;
    y: number = 18;
    w = 172;
    h = 200;

    line_height = 10;

    item_color = new BrowserItemColor()
    focused_color = new BrowserItemColor()
}

export class CoverSetting extends Area {
    enabled = true;

    x: number = 200;
    y: number = 24;
    w = 128;
    h = 96;
}

export class BackgroundSetting {
    x_inc: number = 0
    y_inc: number = 0
}

export class FenrirConfig {
    screens = {
        gamelist: {
            browser: new BrowserSetting(),
            cover: new CoverSetting(),
            backgound: new BackgroundSetting()
        }
    }
}

export const fenrirDefaultConfig = (() => {
    const config = new FenrirConfig()
    config.screens.gamelist.browser.item_color.main_colors.color.rgb32 = 0xffff00
    config.screens.gamelist.browser.item_color.main_colors.shadow.rgb32 = 0x00ffff

    config.screens.gamelist.browser.focused_color.main_colors.color.rgb32 = 0x777777
    config.screens.gamelist.browser.focused_color.main_colors.shadow.rgb32 = 0xff00ff
    return config
})()