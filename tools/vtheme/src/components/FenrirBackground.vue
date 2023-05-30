<script lang="ts">
import { ref } from 'vue'

export default {
  setup() {
    const count = ref(0)

    // expose to template and other options API hooks
    return {
      count
    }
  },

  mounted() {
    console.log(this.count) // 0
  }
}
</script>

<template>
  <div class="columns fenrir-bg">
      <div class="column fenrir-bg-upload">
upload area
      </div>
      <div class="column fenrir-bg-preview">
preview area
      </div>
  </div>
</template>

<style scoped>

</style>
