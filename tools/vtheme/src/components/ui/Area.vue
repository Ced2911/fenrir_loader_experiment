<script lang="ts">
import { ref } from 'vue'
import VueResizable from '@/components/Resizable.vue'

import { Area } from '@/models/screens'

export default {
  components: {
    VueResizable
  },
  setup() {
    // expose to template and other options API hooks
    return {}
  },

  props: {
    area: Area
  },

  emits: ['update'],
  data() {
    const area_rs = {
      width: this.area?.w,
      height: this.area?.h,
      left: this.area?.x,
      top: this.area?.y,
      maxWidth: 512,
      maxHeight: 512,
      minWidth: 10,
      minHeight: 10
    }
    return {
        area_rs
    }
  },
  methods: {
    eHandler(data: any) {
      const o = Object.assign({}, this.area)
      o.h = data.height
      o.w = data.width
      o.x = data.left
      o.y = data.top
      this.$emit('update', o)
    }
  },
  mounted() {}
}
</script>

<template>
  <div class="fenrir-ui-browser">
    <VueResizable v-bind="area_rs" @resize:end="eHandler" @drag:end="eHandler">
      <div class="resizable-content">
      <slot></slot>
    </div>
    </VueResizable>
  </div>
</template>
<style scoped>
.fenrir-ui-browser {
  position: relative;
}
.resizable-content {
  height: 100%;
  width: 100%;
  background-color: aqua;
  opacity: 0.4;
}
</style>
