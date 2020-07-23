function lsfGetSettings() {
  let obj = { method: "lsf_get_settings" };

  window.webkit.messageHandlers.lsfInterface.postMessage(JSON.stringify(obj));
  return JSON.parse(localStorage.getItem('lsfMsg'));
}

function lsfSetSettings(arg) {
  let obj = { method: "lsf_set_settings",
              app_conf: arg };

  window.webkit.messageHandlers.lsfInterface.postMessage(JSON.stringify(obj));
  localStorage.removeItem('lsfMsg');
  return lsfGetSettings();
}

window.addEventListener('unload', function(e) { localStorage.clear(); });
