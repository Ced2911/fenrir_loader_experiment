<script setup lang="ts">
import { useThemeConfigStore } from './store/ThemeConfig';

const store = useThemeConfigStore()
store.init()

</script>

<template>
  <main class="container is-fluid p-0 m-0">
    <nav class="navbar" role="navigation" aria-label="main navigation">
      <div class="navbar-brand">
        <a class="navbar-item" href="https://bulma.io">
          <img src="https://shop.fenrir-ode.fr/img/prestashop-logo-1644698295.jpg" width="28" height="28" />
        </a>
      </div>

      <div id="navbarBasicExample" class="navbar-menu">
        <div class="navbar-start">
          <a class="navbar-item"> Home </a>
          <router-link class="navbar-item" active-class="is-active" to="/game-list">
            New
          </router-link>
          <router-link class="navbar-item" active-class="is-active" to="/game-list">
            Gamelist
          </router-link>
          <router-link class="navbar-item" active-class="is-active" to="/game-list">
            Error
          </router-link>
          <router-link class="navbar-item" active-class="is-active" to="/game-list">
            Sounds
          </router-link>
        </div>

        
      </div>
    </nav>
    <div class="router-content">
      <router-view></router-view>
    </div>
  </main>
</template>

<style scoped></style>
