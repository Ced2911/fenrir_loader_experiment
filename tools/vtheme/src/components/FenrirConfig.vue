<script lang="ts">
import { ref } from 'vue'
import GameList from './screens/GameList.vue'
import AreaUI from '@/components/ui/Area.vue'
import ItemColor from '@/components/ui/ItemColor.vue'
import { fenrirDefaultConfig } from '@/models/screens'
import UploadContent from '@/components/image/UploadContent.vue'

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
      this.browserBgX += this.config.screens.gamelist.backgound.x_inc
      this.browserBgY += this.config.screens.gamelist.backgound.y_inc

      document.documentElement.style.setProperty('--browser-bg-x', '' + this.browserBgX + 'px')
      document.documentElement.style.setProperty('--browser-bg-y', '' + this.browserBgY + 'px')
    }, 16)
  },
  beforeUnmount() {
    clearInterval(this.intervalAnim)
  },
  methods: {
    async browserBackgroundDropped(blobUrl: string) {
      console.log(blobUrl)
      const img: HTMLImageElement = await new Promise((resolve, reject) => {
        const img = document.createElement('img')
        img.onload = () => resolve(img)
        img.onerror = () => reject()
        img.src = blobUrl
      })

      console.log(img.width)
      this.browserBackgroundImage = blobUrl
    },
    updateGamelistFocusColors(c: any) {
      this.config.screens.gamelist.browser.focused_color = c
    },
    updateGamelistItemColors(c: any) {
      this.config.screens.gamelist.browser.item_color = c
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
    }
  },
  computed: {
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
  <div class="fenrir-config">
    <div class="columns">
      <div class="column is-two-thirds min-512">
        <div>
          <UploadContent class="upload-area" @files-dropped="browserBackgroundDropped">
          </UploadContent>
        </div>
        <div
          class="fenrir-config-user-area"
          :style="{ backgroundImage: `url(${browserBackgroundImage})` }"
        >
          <div class="fenrir-config-user-area-img-preview"></div>
          <AreaUI
            v-if="displayAreaGuide"
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
            v-if="displayAreaGuide"
            @update="updateAreaGamelistCover"
            :area="config.screens.gamelist.cover"
          />
        </div>
      </div>
      <div class="fenrir-config-gamelist-browsercolumn"></div>
      <div class="fenrir-config-gamelist-cover column"></div>

      <div class="column box is-one-third">
        <div class="">
          <div class="field">
            <ItemColor
              @update:colors="updateGamelistItemColors"
              :colors="config.screens.gamelist.browser.item_color"
            >
              <template #main-label>
                <label>Main color</label>
              </template>
              <template #gradient-label>
                <label>Gradient color</label>
              </template>
            </ItemColor>

            <ItemColor
              @update:colors="updateGamelistFocusColors"
              :colors="config.screens.gamelist.browser.focused_color"
            >
              <template #main-label>
                <label>Main Focus color</label>
              </template>
              <template #gradient-label>
                <label>Gradient Focus color</label>
              </template>
            </ItemColor>

            <label>Line height </label>
            <div class="control">
              <input
                v-model.number="config.screens.gamelist.browser.line_height"
                type="range"
                max="50"
                min="8"
              />
            </div>

            <label>Area </label>
            <div class="control">
              <input type="checkbox" v-model="displayAreaGuide" />
            </div>

            <label>Background </label>
            <div class="control">
              <input
                @change="updateBackgroundAnimation"
                v-model.number="config.screens.gamelist.backgound.x_inc"
                type="range"
                max="512"
                step="0.25"
                min="-512"
              />
              <input
                @change="updateBackgroundAnimation"
                step="0.25"
                type="number"
                v-model.number="config.screens.gamelist.backgound.x_inc"
              />
            </div>
            <div class="control">
              <input
                @change="updateBackgroundAnimation"
                v-model.number="config.screens.gamelist.backgound.y_inc"
                type="range"
                step="0.25"
                max="512"
                min="-512"
              />
              <input
                @change="updateBackgroundAnimation"
                step="0.25"
                type="number"
                v-model.number="config.screens.gamelist.backgound.y_inc"
              />
            </div>
          </div>
        </div>
      </div>
    </div>
    <div class="columns">
      <div class="column">
        {{ config }}
      </div>
    </div>
  </div>
</template>
<style scoped lang="scss">
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

  &::after,
  &::before {
    background: #ffffff;
    content: ' ';
    position: absolute;
    display: block;
    opacity: 0.2;
    z-index: 65;
  }
  &::after {
    top: 240px;
    left: 0;
    width: 512px;
    height: 272px;
  }
  &::before {
    top: 0;
    left: 356px;
    width: 156px;
    height: 240px;
  }
}
.upload-area {
  width: 512px;
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
  }
}
</style>
