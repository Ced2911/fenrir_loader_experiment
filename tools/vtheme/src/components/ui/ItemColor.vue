<script lang="ts">
import { ref } from 'vue'
import { BrowserItemColor, RgbColor } from '@/models/screens'

export default {
  props: {
    label_color: String,
    colors: BrowserItemColor
  },
  emits: ['update:colors'],
  methods: {
    update() {
      const o = new BrowserItemColor()
      
      o.main_colors.color.rgb32 = RgbColor.fromHexString(this.itemColor.color).rgb32
      o.main_colors.shadow.rgb32 = RgbColor.fromHexString(this.itemColor.shadow).rgb32
      o.gradient_colors.tl.rgb32 = RgbColor.fromHexString(this.itemColor.tl).rgb32
      o.gradient_colors.tr.rgb32 = RgbColor.fromHexString(this.itemColor.tr).rgb32
      o.gradient_colors.br.rgb32 = RgbColor.fromHexString(this.itemColor.br).rgb32
      o.gradient_colors.bl.rgb32 = RgbColor.fromHexString(this.itemColor.bl).rgb32

      this.$emit('update:colors', o)
    }
  },
  data() {
    const itemColor = {
      color: RgbColor.toHexString(this.colors?.main_colors.color),
      shadow: RgbColor.toHexString(this.colors?.main_colors.shadow),
      tl: RgbColor.toHexString(this.colors?.gradient_colors.tl),
      tr: RgbColor.toHexString(this.colors?.gradient_colors.tr),
      br: RgbColor.toHexString(this.colors?.gradient_colors.br),
      bl: RgbColor.toHexString(this.colors?.gradient_colors.bl)
    }
    return { itemColor }
  }
}
</script>

<template>
  <div>
    <slot name="main-label"></slot>
    <div class="control">
      <input @change="update" v-model="itemColor.color" type="color" />
      <input @change="update" v-model="itemColor.shadow" type="color" />
    </div>
  </div>
  <div class="field">
    <slot name="gradient-label"></slot>
    <div class="control">
      <input @change="update" v-model="itemColor.tl" type="color" />
      <input @change="update" v-model="itemColor.tr" type="color" />
      <input @change="update" v-model="itemColor.br" type="color" />
      <input @change="update" v-model="itemColor.bl" type="color" />
    </div>
  </div>
</template>

<style scoped></style>
