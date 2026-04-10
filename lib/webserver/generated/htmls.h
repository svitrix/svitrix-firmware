
// All HTML pages now served from SPA in LittleFS /web/
// Only update_html remains as OTA fallback (critical path — must work without SPA)

static const char update_html[] PROGMEM = R"EOF(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<meta name='viewport' content='width=device-width,initial-scale=1'/>
<title>SVITRIX - Update</title>
<style>body{font-family:system-ui,-apple-system,sans-serif;background:#070b1e;color:#e8e8f0;display:flex;justify-content:center;align-items:center;min-height:100vh;margin:0}.c{background:#0a0e27;border:1px solid #1e2550;border-radius:12px;padding:40px;text-align:center;max-width:480px;width:90%}h2{color:#f0b800;margin-bottom:24px}form{margin:16px 0}input[type=file]{color:#e8e8f0;margin:8px 0}input[type=submit]{background:#f0b800;color:#070b1e;border:none;border-radius:8px;padding:12px 24px;font-size:16px;font-weight:600;cursor:pointer;margin-top:8px}input[type=submit]:hover{background:#e8a800}hr{border-color:#1e2550;margin:24px 0}</style>
</head><body><div class='c'><h2>Firmware Update</h2>
<form method='POST' action='/update' enctype='multipart/form-data'>
<input type='file' accept='.bin,.bin.gz' name='update'><br>
<input type='submit' value='Upload Firmware'>
</form></div></body></html>

)EOF";
