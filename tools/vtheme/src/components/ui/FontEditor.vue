<script lang="ts">
import { FontBuilder, fonts } from '@/services/FontBuilder'
import { useThemeConfigStore } from '@/store/ThemeConfig'
import { mapStores } from 'pinia'

export default {
  emits: ['update:fonts'],
  methods: {
    udpateFont() {
      console.log(this.font)

      this.$emit('update:fonts', { font: this.font, canvas: this.$refs.canvas })
    }
  },
  data() {
    const fontName = fonts.map((f) => f.name)
    return { fonts, fontName, font: '' }
  },
  computed: {
    ...mapStores(useThemeConfigStore),
    _font() {
      return this.themeStore.font
    }
  },
  async mounted() {
    this.themeStore.initFonts(this.$refs.canvas as HTMLCanvasElement)
    this.font = this.themeStore.font

    this.$emit('update:fonts', { font: this.font, canvas: this.$refs.canvas })
  }
}
</script>

<template>
  <div>
    <div class="font-canvas">
      <div class="field">
        <div class="control">
          <label class="label"> Font </label>
          <div class="select is-rounded">
            <select @change="udpateFont" v-model="font">
              <option v-for="fontn in fonts" :key="fontn.name" :value="fontn.name">
                {{ fontn.name }} - {{ fontn.size }}px
              </option>
            </select>
          </div>
        </div>
      </div>
      <canvas ref="canvas" width="2048" height="32"></canvas>
      <svg width="0" height="0" style="position: absolute; z-index: -1">
        <defs>
          <filter id="remove-alpha" x="0" y="0" width="100%" height="100%">
            <feComponentTransfer>
              <feFuncA type="discrete" tableValues="0 1"></feFuncA>
            </feComponentTransfer>
          </filter>
        </defs>
      </svg>
    </div>
  </div>
</template>

<style lang="scss" scoped>
.font-canvas {
  canvas {
    display: none;
    image-rendering: pixelated;
    image-rendering: crisp-edges;
    image-rendering: pixelated;
    font-smooth: never;
    -webkit-font-smoothing: none;
    position: absolute;
  }
}
</style>
