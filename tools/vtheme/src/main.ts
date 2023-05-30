import './assets/main.scss'
import "../node_modules/bulma/bulma.sass"
import 'verte/dist/verte.css'
import { createRouter, createWebHistory, createWebHashHistory } from 'vue-router'

import { createApp } from 'vue'

import App from './App.vue'

const router = createRouter({
    // 4. Provide the history implementation to use. We are using the hash history for simplicity here.
    history: createWebHashHistory(),
    routes: [
        {
          path: '/',
          component: () => import('@/pages/Home.vue'),
        },
        {
          path: '/game-list',
          component: () => import('@/pages/screens/GameList.vue'),
        },
      ],
  })
  

const app = createApp(App)
app.use(router)
app.mount('#app')
