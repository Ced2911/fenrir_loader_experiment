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
 <fenrir-config />
</template>
