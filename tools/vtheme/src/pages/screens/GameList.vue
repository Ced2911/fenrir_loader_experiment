<script lang="ts">
import FenrirBackground from '@/components/FenrirBackground.vue'
import FenrirConfig from '@/components/FenrirConfig.vue'

export default {
  components: {
    FenrirConfig,
    FenrirBackground
  },
  setup() {
    // expose to template and other options API hooks
    return {}
  },

  data() {
    return {
      
    }
  },
  mounted() {}
}
</script>

<template>
  <div class="columns">
    <div class="column">
      <fenrir-config />
    </div>
  </div>
</template>
