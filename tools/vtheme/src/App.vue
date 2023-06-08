<script setup lang="ts"></script>

<template>
  <main class="container is-fluid p-0 m-0">
    <!--<nav class="navbar" role="navigation" aria-label="main navigation">
      <div class="navbar-brand">
        <a class="navbar-item" href="https://bulma.io">
          Fenrir Configuration
          <img src="https://bulma.io/images/bulma-logo.png" width="112" height="28" />
        </a>

        <a
          role="button"
          class="navbar-burger"
          aria-label="menu"
          aria-expanded="false"
          data-target="navbarBasicExample"
        >
          <span aria-hidden="true"></span>
          <span aria-hidden="true"></span>
          <span aria-hidden="true"></span>
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

        <div class="navbar-end">
          <div class="navbar-item">
            <div class="field is-grouped">
              <p class="control">
                <a
                  class="bd-tw-button button"
                  data-social-network="Twitter"
                  data-social-action="tweet"
                  data-social-target="https://bulma.io"
                  target="_blank"
                  href="https://twitter.com/intent/tweet?text=Bulma: a modern CSS framework based on Flexbox&amp;hashtags=bulmaio&amp;url=https://bulma.io&amp;via=jgthms"
                >
                  <span class="icon">
                    <i class="fab fa-twitter"></i>
                  </span>
                  <span> Tweet </span>
                </a>
              </p>
              <p class="control">
                <a
                  class="button is-primary"
                  href="https://github.com/jgthms/bulma/releases/download/0.9.4/bulma-0.9.4.zip"
                >
                  <span class="icon">
                    <i class="fas fa-download"></i>
                  </span>
                  <span>Download</span>
                </a>
              </p>
            </div>
          </div>
        </div>
      </div>
    </nav>-->
    <div class="router-content">
      <router-view></router-view>
    </div>
  </main>
</template>

<style scoped></style>
