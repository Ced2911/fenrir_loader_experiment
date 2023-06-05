<script lang="ts">
import { ref } from 'vue'
import GameList from './screens/GameList.vue'
import AreaUI from '@/components/ui/Area.vue'
import ItemColor from '@/components/ui/ItemColor.vue'
import { fenrirDefaultConfig } from '@/models/screens'
import UploadContent from '@/components/image/UploadContent.vue'
import ImageTiler from '@/services/VDP2Tiler'
import { downloadBuffer, saveToLocalStorage, getFromLocalStorage, RGBFunc } from '@/services/Utils'
import { ThemeExport, ThemeConfigToBuffer, THEME_ID } from '@/services/ExportFenrirThemeConfig'

import type { FenrirConfig } from '@/models/screens'
import { plainToClass } from 'class-transformer'

const SCREEN_W = 352
const SCREEN_H = 240
const SCREEN_MAP_SZ = 512

export default {
  components: {
    //GameList,
    AreaUI,
    ItemColor,
    UploadContent
  },
  setup() {
    return {}
  },
  mounted() {
    this.intervalAnim = setInterval(() => {
      this.browserBgX -= this.config.screens.gamelist.backgound.x_inc
      this.browserBgY -= this.config.screens.gamelist.backgound.y_inc

      document.documentElement.style.setProperty(
        '--browser-bg-x',
        '' + ((SCREEN_MAP_SZ - SCREEN_W) / 2 + this.browserBgX) + 'px'
      )
      document.documentElement.style.setProperty(
        '--browser-bg-y',
        '' + ((SCREEN_MAP_SZ - SCREEN_H) / 2 + this.browserBgY) + 'px'
      )
    }, 16)

    this.restoreFromLocalStorage()

    this.udpateCSS()
  },
  beforeUnmount() {
    clearInterval(this.intervalAnim)
  },
  methods: {
    udpateCSS() {
      document.documentElement.style.setProperty(
        '--browser-main-color',
        RGBFunc.toHexString(this.config.screens.gamelist.browser.item_color.main_colors.color)
      )

      Object.entries(this.config.screens.gamelist.browser.item_color.gradient_colors).forEach(
        ([v, c]) => {
          document.documentElement.style.setProperty(
            '--browser-gr-' + v,

            RGBFunc.toHexString(
              RGBFunc.multiplyColor(
                this.config.screens.gamelist.browser.item_color.main_colors.color,
                c
              )
            )
          )
        }
      )
    },
    restoreFromLocalStorage() {
      // try to get data from ls
      const config_ls = getFromLocalStorage('theme-config')
      console.log(config_ls)
      if (config_ls) {
        const config = JSON.parse(config_ls)
        this.config = config as FenrirConfig
      }

      const gl_bg = getFromLocalStorage('gamelist-bg')

      if (gl_bg) {
        const ii = Uint8Array.from(gl_bg, (c) => c.charCodeAt(0))
        const blobUrl = URL.createObjectURL(new Blob([ii]))

        const img = document.createElement('img')
        img.src = blobUrl

        this.browserBackgroundImage = blobUrl
      }
    },

    async browserBackgroundDropped(blob: Blob) {
      const blobUrl = URL.createObjectURL(blob)

      const img: HTMLImageElement = await new Promise((resolve, reject) => {
        const img = document.createElement('img')
        img.onload = () => resolve(img)
        img.onerror = () => reject()
        img.src = blobUrl
      })

      const imgB64 = await new Promise((resolve, _) => {
        const reader = new FileReader()
        reader.onloadend = () => resolve(reader.result)
        reader.readAsDataURL(blob)
      })

      console.log(img.width)
      this.browserBackgroundImage = blobUrl

      if (imgB64) {
        const d = imgB64.split(',')

        saveToLocalStorage('gamelist-bg', atob(d[1]))
      }
    },
    updateGamelistFocusColors(c: any) {
      this.config.screens.gamelist.browser.focused_color = c
      this.udpateCSS()
    },
    updateGamelistItemColors(c: any) {
      this.config.screens.gamelist.browser.item_color = c
      this.udpateCSS()
    },
    updateAreaGamelistBrowser(c: any) {
      Object.assign(this.config.screens.gamelist.browser, c)
    },
    updateAreaGamelistCover(c: any) {
      Object.assign(this.config.screens.gamelist.cover, c)
    },
    updateBackgroundAnimation() {
      this.browserBgX = 0
      this.browserBgY = 0
    },
    async buildImage() {
      const b = await ImageTiler(this.browserBackgroundImage)
      downloadBuffer(b, 'vdp2.dat')
    },
    async buildTheme() {
      const vdpbg = await ImageTiler(this.browserBackgroundImage)
      const th = ThemeConfigToBuffer(this.config)

      const expt = new ThemeExport()
      expt.addRessource(THEME_ID.VDP2_BG, vdpbg)
      expt.addRessource(THEME_ID.THEME_CONFIG_V0, th)

      downloadBuffer(expt.build(), 'theme.bin')

      saveToLocalStorage('theme-config', JSON.stringify(this.config))
    }
  },
  computed: {
    configJson: {
      get(): string {
        return JSON.stringify(this.config, null, 2)
      },
      set(v: string) {
        this.config = JSON.parse(v)
      }
    },

    fakeGamesList() {
      const max_g = Math.floor(
        this.config.screens.gamelist.browser.h / this.config.screens.gamelist.browser.line_height
      )
      return [
        'Burning Rangers',
        'Panzer Dragoon Saga',
        'Virtua Fighter 2',
        'Akumajou Dracula X: Gekka no Yasoukyoku',
        'Asuka 120% Limited Burning Fest. Limited',
        'Bakuretsu Hunter R',
        'Battle Arena Toshinden Remix',
        'Bomberman Wars',
        'Bubble Bobble featuring Rainbow Islands',
        'Cyberbots: Full Metal Madness',
        'Daytona USA',
        'Digital Dance Mix Vol.1 Namie Amuro',
        'DragonHeart: Fire & Steel',
        'Eve: Burst Error',
        'Fighting Vipers',
        'Galaxy Fight',
        'Garou Densetsu 3: Road to the Final Victory',
        'Golden Axe: The Duel',
        'Gungriffon II',
        'Hokuto no Ken',
        'Keio Flying Squadron 2',
        "King of Fighters '97, The",
        'Last Bronx',
        'Layer Section II',
        'Linkle Liver Story',
        'Lunar: Silver Star Story Complete',
        'Magical Drop III'
      ].splice(0, max_g)
    }
  },
  data() {
    return {
      intervalAnim: 0,
      browserBackgroundImage: '',
      config: fenrirDefaultConfig,
      browserBgX: 0,
      browserBgY: 0,
      displayAreaGuide: true
    }
  }
}
</script>

<template>
  <div class="fenrir-config columns m-0">
    <div class="column is-one-fifth has-background-dark">
      <UploadContent class="upload-area" @files-dropped="browserBackgroundDropped">
        <div class="is-size-7">File must be 512x512 with less than 16 colors</div>
      </UploadContent>

      <span class="is-size-5">Debug</span>
      <textarea rows="10" class="textarea" v-model="configJson"></textarea>
    </div>
    <div class="column has-background-black">
      <div class="fenrir-config-preview-area">
        <div
          class="fenrir-config-user-area my-0 mx-auto"
          :style="{ backgroundImage: `url(${browserBackgroundImage})` }"
        >
          <div class="fenrir-config-user-area-img-preview"></div>
          <div class="fenrir-config-user-area-control">
            <AreaUI
              :active="displayAreaGuide"
              @update="updateAreaGamelistBrowser"
              :area="config.screens.gamelist.browser"
            >
              <div class="game-list">
                <ul :style="{ lineHeight: config.screens.gamelist.browser.line_height + 'px' }">
                  <li :key="g" v-for="g in fakeGamesList">{{ g }}</li>
                </ul>
              </div>
            </AreaUI>
            <AreaUI
              :active="displayAreaGuide"
              @update="updateAreaGamelistCover"
              :area="config.screens.gamelist.cover"
              ><div class="cover-area"></div>
            </AreaUI>
          </div>
        </div>
      </div>
    </div>

    <div class="column is-one-fifth has-background-dark">
      <div class="">
        <div class="field">
          <ItemColor
            @update:colors="updateGamelistItemColors"
            :colors="config.screens.gamelist.browser.item_color"
          >
            <template #main-label>
              <label class="label">Main color</label>
            </template>
            <template #gradient-label>
              <label class="label">Gradient color</label>
            </template>
          </ItemColor>

          <ItemColor
            @update:colors="updateGamelistFocusColors"
            :colors="config.screens.gamelist.browser.focused_color"
          >
            <template #main-label>
              <label class="label">Main Focus color</label>
            </template>
            <template #gradient-label>
              <label class="label">Gradient Focus color</label>
            </template>
          </ItemColor>

          <div class="field">
            <div class="control">
              <label class="label">Line height </label>
              <input
                class="input"
                v-model.number="config.screens.gamelist.browser.line_height"
                type="number"
                max="50"
                min="8"
              />
            </div>
          </div>

          <div class="field is-horizontal">
            <div class="control">
              <label class="label"> Display/Move area helpers</label>
            </div>
            <div class="control">
              <label class="checkbox"
                >&nbsp;
                <input type="checkbox" v-model="displayAreaGuide" />
              </label>
            </div>
          </div>

          <label class="label">Background scrolling</label>
          <div class="field is-horizontal">
            <div class="field-body">
              <div class="field has-addons">
                <div class="control">
                  <span class="button is-static"> X </span>
                </div>
                <div class="control">
                  <input
                    @change="updateBackgroundAnimation"
                    step="0.25"
                    type="number"
                    class="input"
                    v-model.number="config.screens.gamelist.backgound.x_inc"
                  />
                </div>
              </div>
              <div class="field has-addons">
                <div class="control">
                  <span class="button is-static"> Y </span>
                </div>
                <div class="control">
                  <input
                    @change="updateBackgroundAnimation"
                    step="0.25"
                    type="number"
                    class="input"
                    v-model.number="config.screens.gamelist.backgound.y_inc"
                  />
                </div>
              </div>
            </div>
          </div>
        </div>

        <div class="field">
          <button class="button is-primary" @click="buildTheme">
            <span class="icon">
              <font-awesome-icon icon="fa-solid fa-bolt" />
            </span>
            <span>Build theme</span>
          </button>
        </div>
      </div>
    </div>
  </div>
</template>
<style scoped lang="scss">
@use 'sass:math';
$screen-w: var(--screen-w, 352px);
$screen-h: var(--screen-h, 240px);
$screen-map-sz: var(--screen-map-sz, 512px);
.fenrir-config {
  flex: 1;
}

.min-512 {
  min-width: 512px;
  padding: 0;
}
.resizable-content {
  height: 100%;
  width: 100%;
  background-color: aqua;
  opacity: 0.4;
}

@keyframes browserBackgroundAnimation {
  0% {
    background-position-x: 0;
    background-position-y: 0;
  }
  100% {
  }
}
.fenrir-config-user-area {
  background-image: url('@/assets/dbg.png');
  width: 512px;
  height: 512px;
  position: relative;
  background: #ccc;
  background-repeat: repeat;

  background-position-x: var(--browser-bg-x);
  background-position-y: var(--browser-bg-y);

  animation: background-position-x 16ms linear;
  animation: background-position-y 16ms linear;
}

.fenrir-config-user-area-control {
  transform: translate(
    calc(($screen-map-sz - $screen-w) * 0.5),
    calc(($screen-map-sz - $screen-h) * 0.5)
  );

  border: 1px solid #555;

  width: $screen-w;
  height: $screen-h;

  &::before {
    // background: #ffffff;
    content: ' ';
    position: absolute;
    display: block;
    opacity: 0.2;
    z-index: 65;
    width: $screen-map-sz;
    height: $screen-map-sz;

    top: 0;
    left: 0;
    transform: translate(
      calc(($screen-map-sz - $screen-w) * -0.5),
      calc(($screen-map-sz - $screen-h) * -0.5)
    );

    border-bottom: calc(($screen-map-sz - $screen-h) * 0.5) solid #ffffff;
    border-top: calc(($screen-map-sz - $screen-h) * 0.5) solid #ffffff;
    border-left: calc(($screen-map-sz - $screen-w) * 0.5) solid #ffffff;
    border-right: calc(($screen-map-sz - $screen-w) * 0.5) solid #ffffff;
  }
}

.upload-area {
  //width: 512px;
}
.fenrir-config-user-area-img-preview {
  position: absolute;
}
.game-list {
  li {
    font-size: 12px;
    font-smooth: never;
    -webkit-font-smoothing: none;
    text-overflow: clip;
    display: block;

    white-space: nowrap;
    overflow: hidden;
    color: var(--browser-main-color);
    background: radial-gradient(ellipse at top left, var(--browser-gr-tl) 15%, transparent 60%),
      radial-gradient(ellipse at bottom left, var(--browser-gr-bl) 15%, transparent 60%),
      radial-gradient(ellipse at top right, var(--browser-gr-tr) 15%, transparent 70%),
      radial-gradient(ellipse at bottom right, var(--browser-gr-br) 15%, transparent 70%);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
  }
}
.cover-area {
  background-color: #333;
  height: 100%;
}
</style>
