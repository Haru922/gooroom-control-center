function lsf_get_settings() {
  let obj = { method: "get_settings" };

  window.webkit.messageHandlers.lsf_interface.postMessage(JSON.stringify(obj));
  return JSON.parse(localStorage.getItem('lsf_msg');
}

function lsf_set_settings(arg) {
  let obj = { method: "set_settings",
              app_conf: arg };

  window.webkit.messageHandlers.lsf_interface.postMessage (JSON.stringify(obj));
  localStorage.removeItem('lsf_msg');
  lsf_get_settings();
}

window.addEventListener('unload', function(e) { localStorage.clear(); });
