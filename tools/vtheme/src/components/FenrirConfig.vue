<script lang="ts">
import { mapStores } from 'pinia'

import AreaUI from '@/components/ui/Area.vue'
import ItemColor from '@/components/ui/ItemColor.vue'
import FontEditor from '@/components/ui/FontEditor.vue'
import BackgroundSettings from '@/components/ui/BackgroundSetting.vue'
import UploadContent from '@/components/image/UploadContent.vue'

import { useThemeConfigStore } from '@/store/ThemeConfig'
import type { FenrirConfig } from '@/models/screens'

import { RGBFunc, SetCssVar } from '@/services/Utils'
import { fonts } from '@/services/FontBuilder'

const SCREEN_W = 352
const SCREEN_H = 240
const SCREEN_MAP_SZ = 512

export default {
  components: {
    //GameList,
    AreaUI,
    ItemColor,
    UploadContent,
    FontEditor,
    BackgroundSettings
  },
  setup() {
    return {}
  },
  mounted() {
    this.intervalAnim = setInterval(() => {
      this.browserBgX -= this.config.screens.gamelist.backgound.x_inc
      this.browserBgY -= this.config.screens.gamelist.backgound.y_inc

      this.browserFgX -= this.config.screens.gamelist.foreground.x_inc
      this.browserFgY -= this.config.screens.gamelist.foreground.y_inc
    }, 16)

    this.browserBackgroundImage = this.themeStore.backgroundImageUrl
    this.browserForegroundImage = this.themeStore.foregroundImageUrl

    this.themeStore.$subscribe(() => {
      this.udpateCSS()
    })

    this.udpateCSS()
  },
  beforeUnmount() {
    clearInterval(this.intervalAnim)
  },
  methods: {
    udpateCSS() {
      const style = document.documentElement.style
      const { item_color, focused_color } = this.config.screens.gamelist.browser

      const colors = {
        '--browser-main-color': RGBFunc.toHexString(item_color.main_colors.color),
        '--browser-main-shadow': RGBFunc.toHexString(item_color.main_colors.shadow),
        '--browser-focused-color': RGBFunc.toHexString(focused_color.main_colors.color),
        '--browser-focused-shadow': RGBFunc.toHexString(focused_color.main_colors.shadow)
      }

      Object.entries(colors).forEach(([v, color]) => {
        style.setProperty(v, color)
      })

      Object.entries(item_color.gradient_colors).forEach(([v, c]) => {
        document.documentElement.style.setProperty(
          '--browser-gr-' + v,
          RGBFunc.toHexString(RGBFunc.multiplyColor(item_color.main_colors.color, c))
        )
      })

      Object.entries(focused_color.gradient_colors).forEach(([v, c]) => {
        document.documentElement.style.setProperty(
          '--browser-fr-' + v,
          RGBFunc.toHexString(RGBFunc.multiplyColor(focused_color.main_colors.color, c))
        )
      })

      SetCssVar('--foreground-image', `url(${this.themeStore.foregroundImageUrl})`)
    },

    async browserBackgroundDropped(blob: Blob) {
      const blobUrl = URL.createObjectURL(blob)
      this.browserBackgroundImage = blobUrl

      this.themeStore.updateBackground(blob)
    },
    async browserForegroundDropped(blob: Blob) {
      const blobUrl = URL.createObjectURL(blob)
      this.browserForegroundImage = blobUrl

      this.themeStore.updateForeground(blob)
    },

    resetBackgroundAnimation() {
      this.browserBgX = 0
      this.browserBgY = 0
    },
    resetForegroundAnimation() {
      this.browserFgX = 0
      this.browserFgY = 0
    },

    async updateFonts({ font, canvas }: { font: string; canvas: HTMLCanvasElement }) {
      await this.themeStore.updateFont(font)
    }
  },
  computed: {
    ...mapStores(useThemeConfigStore),
    configJson: {
      get(): string {
        return JSON.stringify(this.config, null, 2)
      },
      set(v: string) {
        this.config = JSON.parse(v)
      }
    },

    browserFont(): any {
      const fontInfo = fonts.find((f) => f.name === this.themeStore.font)
      if (fontInfo) {
        return {
          fontFamily: fontInfo.name,
          fontSize: fontInfo.size + 'px'
        }
      }
      return {}
    },

    browserBackgroundPosition(): { x: string; y: string } {
      return {
        x: (SCREEN_MAP_SZ - SCREEN_W) / 2 + this.browserBgX + 'px',
        y: (SCREEN_MAP_SZ - SCREEN_H) / 2 + this.browserBgY + 'px'
      }
    },
    browserForegroundPosition(): { x: string; y: string } {
      return {
        x: (SCREEN_MAP_SZ - SCREEN_W) / 2 + this.browserFgX + 'px',
        y: (SCREEN_MAP_SZ - SCREEN_H) / 2 + this.browserFgY + 'px'
      }
    },

    config(): FenrirConfig {
      return this.themeStore.config
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
    //console.log(this.themeStore)
    return {
      intervalAnim: 0,
      browserBackgroundImage: '',
      browserForegroundImage: '',
      //config: this.themeStore.config,
      browserBgX: 0,
      browserBgY: 0,
      browserFgX: 0,
      browserFgY: 0,
      displayAreaGuide: true,
      fontBuffer: null,
      scale: 100
    }
  }
}
</script>

<template>
  <div class="fenrir-config columns m-0">
    <div class="column is-one-fifth has-background-dark">
      <UploadContent class="upload-area" @files-dropped="browserBackgroundDropped" buttonlabel="Upload background">
        <div class="is-size-7">File must be 512x512 with less than 16 colors</div>
      </UploadContent>
      
      <UploadContent class="upload-area" @files-dropped="browserForegroundDropped"  buttonlabel="Upload foreground">
        <div class="is-size-7">File must be 512x512 with less than 16 colors</div>
      </UploadContent>
      
      <BackgroundSettings @change="resetBackgroundAnimation" v-model:model-value="themeStore.config.screens.gamelist.backgound" label="Background scrolling"/>
      <BackgroundSettings @change="resetForegroundAnimation" v-model:model-value="themeStore.config.screens.gamelist.foreground" label="Foreground scrolling"/>

      <div class="field">
        <div class="control">
          <label class="label">
            Display/Move area helpers
            <input type="checkbox" v-model="displayAreaGuide" />
          </label>
        </div>
      </div>

      <span class="is-size-5">Debug</span>
      <textarea rows="10" class="textarea" v-model="configJson"></textarea>
    </div>
    <div class="column has-background-black">
      <div class="fenrir-config-preview-area">
        <div
          class="fenrir-config-user-area my-0 mx-auto"
          :style="{
            backgroundImage: `url(${browserBackgroundImage})`,
            backgroundPositionX: browserBackgroundPosition.x,
            backgroundPositionY: browserBackgroundPosition.y,
            transform: `scale(${scale/100})`
          }"
        >
          <div
            :style="{
              backgroundImage: `url(${browserForegroundImage})`,
              backgroundPositionX: browserForegroundPosition.x,
              backgroundPositionY: browserForegroundPosition.y,
              
            }"
            class="fenrir-config-foreground"
          ></div>
          <div class="fenrir-config-user-area-img-preview"></div>
          <div class="fenrir-config-user-area-control">
            <AreaUI
              :active="displayAreaGuide"
              @update="themeStore.updateAreaGamelistBrowser"
              :area="config.screens.gamelist.browser"
            >
              <div class="game-list">
                <ul :style="{ lineHeight: config.screens.gamelist.browser.line_height + 'px' }">
                  <li :style="{ ...browserFont }" :key="g" v-for="g in fakeGamesList">{{ g }}</li>
                </ul>
              </div>
            </AreaUI>
            <AreaUI
              :active="displayAreaGuide"
              @update="themeStore.updateAreaGamelistCover"
              :area="config.screens.gamelist.cover"
              ><div class="cover-area"></div>
            </AreaUI>
          </div>
        </div>
      </div>
      <div>
        <input v-model="scale" type="range" min="10" max="300" />
      </div>
    </div>

    <div class="column is-one-fifth has-background-dark">
      <div class="">
        <div class="field">
          <ItemColor
            @update:colors="themeStore.updateGamelistItemColors"
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
            @update:colors="themeStore.updateGamelistFocusColors"
            :colors="config.screens.gamelist.browser.focused_color"
          >
            <template #main-label>
              <label class="label">Main Focus color</label>
            </template>
            <template #gradient-label>
              <label class="label">Gradient Focus color</label>
            </template>
          </ItemColor>

          <FontEditor @update:fonts="updateFonts"></FontEditor>

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
        </div>

        <div class="field">
          <button class="button is-primary" @click="themeStore.buildTheme">
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

  animation: background-position-x 16ms linear;
  animation: background-position-y 16ms linear;

  .fenrir-config-foreground {
    display: block;
    width: 100%;
    height: 100%;
    background-image: var(--foreground-image);
    left: attr(foregroundpositionx);
    top: attr(foregroundpositiony);
    position: absolute;
  }
}

.fenrir-config-preview-area {
    display: flex;
    height: -webkit-fill-available;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    position: relative;
    overflow: hidden;
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

    &:nth-of-type(5) {
      color: var(--browser-focused-color);
      background: radial-gradient(ellipse at top left, var(--browser-fr-tl) 15%, transparent 60%),
        radial-gradient(ellipse at bottom left, var(--browser-fr-bl) 15%, transparent 60%),
        radial-gradient(ellipse at top right, var(--browser-fr-tr) 15%, transparent 70%),
        radial-gradient(ellipse at bottom right, var(--browser-fr-br) 15%, transparent 70%);

      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
    }
  }
}
.cover-area {
  background-color: #333;
  height: 100%;
}
</style>
