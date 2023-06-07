<script lang="ts">
import { FontBuilder } from '@/services/FontBuilder'
import { useThemeConfigStore } from '@/store/ThemeConfig'
import { mapStores } from 'pinia'

export default {
  emits: ['update:fonts'],
  methods: {},
  data() {
    return {}
  },
  computed: {
    ...mapStores(useThemeConfigStore),
  },
  async mounted() {
    this.themeStore.initFonts(this.$refs.canvas as HTMLCanvasElement)
    this.$emit('update:fonts', {font: this.themeStore.font, canvas:this.$refs.canvas})
    
  }
}
</script>

<template>
  <div>
    <div class="font-canvas">
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
  height: 64px;
  canvas {
    image-rendering: pixelated;
    image-rendering: crisp-edges;
    image-rendering: pixelated;
    font-smooth: never;
    -webkit-font-smoothing: none;
    position: absolute;
  }
}
</style>
