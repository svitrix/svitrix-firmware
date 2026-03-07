
/*
 * Auto-generated file. Do not modify by hand.
 * Run: python3 tools/web_compress.py
 *
 * This HTML code will be injected in /setup webpage using a <div></div> element as parent
 * The parent element will have the HTML id properties equal to 'raw-html-<id>'
 * where the id value will be equal to the id parameter passed to the function addHTML(html_code, id).
 */

static const char custom_html[] PROGMEM = R"EOF(
<div id="iconcontent">
        <div id="form-con">
            <form>
                <label for="lametric-iconID">Icon ID</label><br>
                <input type="text" id="lametric-iconID" name="lametric-iconID"><br>
                <div class="button-row">
                    <input class="btn" type="button" value="Preview" onclick="createLametricLink()">
                    <input class="btn" type="button" value="Download" onclick="downloadLametricImage()">
                </div>
            </form>
        </div>
        <br>
        <br>
        <div id="icon-container">
        </div>
    </div>
)EOF";

static const char custom_css[] PROGMEM = R"EOF(
.iconcontent {
            width: 50%;
            justify-content: center;
        }
        #form-con {
            width: 50%;
            margin: 0 auto;
            min-width: 200px;
        }
        #form-con label {
            color: #8890b0;
            font-size: 13px;
        }
        #form-con input[type=text] {
            background: #111638;
            border: 1px solid #1e2550;
            color: #e8e8f0;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 14px;
            width: 100%;
        }
        #form-con input[type=text]:focus {
            border-color: #f0b800;
            box-shadow: 0 0 0 2px rgba(240,184,0,.25);
            outline: 0;
        }
        .button-row input {
            width: 50%;
            margin: 0 5px;
        }
        .button-row {
            display: flex;
            justify-content: space-evenly;
            margin: 0 -5px;
            margin-top: 5px;
        }
        #icon-container {
            margin: 0 auto;
            max-width: 150px;
            max-height: 150px;
            width: 150px;
            background-color: black;
            height: 150px;
            margin: 0 auto;
            border-radius: 8px;
            border: 1px solid #1e2550;
        }
        #icon-container img {
            image-rendering: pixelated;
            max-width: 150px;
            max-height: 150px;
            width: 150px;
            background-color: black;
            height: 150px;
            border-radius: 8px;
        }
)EOF";

static const char custom_script[] PROGMEM = R"EOF(
function createLametricLink(){const e=document.getElementById("lametric-iconID").value,t=document.createElement("img");t.onerror=function(){openModalMessage("Error","<b>This ID doesnt exist</b>")},t.src="https://developer.lametric.com/content/apps/icon_thumbs/"+e;const n=document.getElementById("icon-container");n.innerHTML="",n.appendChild(t)}async function downloadLametricImage(){const e=document.getElementById("lametric-iconID").value;try{let n=await fetch("https://developer.lametric.com/content/apps/icon_thumbs/"+e),o=await n.blob();var t="";const c=n.headers.get("content-type");if("image/jpeg"===c||"image/png"===c){t=".jpg";let n=new Image,c=URL.createObjectURL(o);n.onload=function(){let o=document.createElement("canvas");o.width=n.width,o.height=n.height,o.getContext("2d").drawImage(n,0,0,n.width,n.height),o.toBlob((function(n){sendBlob(n,e,t)}),"image/jpeg",1),URL.revokeObjectURL(c)},n.src=c}else"image/gif"===n.headers.get("content-type")&&sendBlob(o,e,t=".gif")}catch(e){console.log("Error"),openModalMessage("Error","<b>This ID doesnt exist</b>")}}function sendBlob(e,t,n){const o=new FormData;o.append("image",e,"ICONS/"+t+n),fetch("/edit",{method:"POST",body:o,mode:"no-cors"}).then((e=>{e.ok&&openModalMessage("Finish","<b>Icon saved</b>")})).catch((e=>{console.log(e)}))}
)EOF";



static const char screen_html[] PROGMEM = R"EOF(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8" />
  <title>SVITRIX - LiveView</title>

  <style>
    body, #container {
      margin: 0;
      padding: 0;
      display: flex;
      background: #070b1e;
    }
    body {
      justify-content: center;
      align-items: center;
      flex-direction: column;
      min-height: 100vh;
      font-family: system-ui, -apple-system, sans-serif;
    }
    #container {
      position: relative;
      padding: 40px;
      border: 1px solid #1e2550;
      border-radius: 12px;
      box-shadow: 0 0 30px rgba(240, 184, 0, 0.08);
    }
    canvas {
      position: relative;
      max-width: 100%;
      max-height: 100%;
      background: #000;
      z-index: 1;
      border-radius: 4px;
    }
    .controls {
      display: flex;
      justify-content: center;
      align-items: center;
      gap: 12px;
      margin-top: 24px;
    }
    .controls button {
      width: 150px;
      height: 44px;
      color: #f0b800;
      background: #111638;
      border: 1px solid #1e2550;
      border-radius: 8px;
      font-size: 14px;
      cursor: pointer;
      transition: all 0.2s;
    }
    .controls button:hover {
      background: #f0b800;
      color: #070b1e;
      border-color: #f0b800;
    }
  </style>

  <script type="module">
    import { GIFEncoder, quantize, applyPalette } from 'https://unpkg.com/gifenc@1.0.3';

    const canvas = document.getElementById('canvas');
    const ctx = canvas.getContext('2d');
    const WIDTH = 1052;
    const HEIGHT = 260;

    canvas.width = WIDTH;
    canvas.height = HEIGHT;

    let gifEncoder;
    let isRecording = false;
    let lastFrameTime = performance.now();

    /* ── Live screen polling ── */

    function fetchAndDraw() {
      fetch("/api/screen")
        .then(function (res) { return res.json(); })
        .then(function (pixels) {
          ctx.clearRect(0, 0, canvas.width, canvas.height);
          ctx.fillStyle = "#000";

          for (let row = 0; row < 8; row++) {
            for (let col = 0; col < 32; col++) {
              const color = pixels[row * 32 + col];
              const r = (color & 0xff0000) >> 16;
              const g = (color & 0x00ff00) >> 8;
              const b = color & 0x0000ff;
              ctx.fillStyle = `rgb(${r},${g},${b})`;
              ctx.fillRect(col * 33, row * 33, 29, 29);
            }
          }

          // Capture frame if recording GIF
          if (isRecording) {
            const now = performance.now();
            const delay = Math.round(now - lastFrameTime);
            lastFrameTime = now;

            const imageData = ctx.getImageData(0, 0, WIDTH, HEIGHT).data;
            const format = "rgb444";
            const palette = quantize(imageData, 256, { format: format });
            const index = applyPalette(imageData, palette, format);
            gifEncoder.writeFrame(index, WIDTH, HEIGHT, { palette: palette, delay: delay });
          }

          fetchAndDraw();
        });
    }

    /* ── File download helper ── */

    function downloadBlob(data, filename, mimeType) {
      const blob = data instanceof Blob ? data : new Blob([data], { type: mimeType });
      const url = URL.createObjectURL(blob);
      const link = document.createElement("a");
      link.href = url;
      link.download = filename;
      link.click();
    }

    /* ── Button handlers ── */

    document.addEventListener("DOMContentLoaded", function () {
      fetchAndDraw();

      // Download PNG
      document.getElementById("downloadPng").addEventListener("click", function () {
        const link = document.createElement("a");
        link.href = canvas.toDataURL();
        link.download = 'svitrix.png';
        link.click();
      });

      // Next app
      document.getElementById("nextApp").addEventListener("click", function () {
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/api/nextapp", true);
        xhr.send();
      });

      // Previous app
      document.getElementById("prevApp").addEventListener("click", function () {
        var xhr = new XMLHttpRequest();
        xhr.open("POST", "/api/previousapp", true);
        xhr.send();
      });

      // GIF recording toggle
      document.getElementById("recordGif").addEventListener("click", async function () {
        var btn = this;
        if (isRecording) {
          gifEncoder.finish();
          var bytes = gifEncoder.bytesView();
          downloadBlob(bytes, 'svitrix.gif', 'image/gif');
          isRecording = false;
          btn.textContent = "Start GIF recording";
        } else {
          gifEncoder = GIFEncoder();
          lastFrameTime = performance.now();
          isRecording = true;
          btn.textContent = "Stop GIF recording";
        }
      });
    });
  </script>
</head>

<body>
  <div id="container">
    <canvas id="canvas"></canvas>
  </div>
  <div class="controls">
    <button id="prevApp">&lt;</button>
    <button id="downloadPng">Download PNG</button>
    <button id="recordGif">Start GIF recording</button>
    <button id="nextApp">&gt;</button>
  </div>
</body>
</html>

)EOF";



static const char backup_html[] PROGMEM = R"EOF(
<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>SVITRIX - Backup</title><script src="https://cdnjs.cloudflare.com/ajax/libs/jszip/3.1.5/jszip.min.js"></script><style>*{box-sizing:border-box;margin:0;padding:0}body{font-family:system-ui,-apple-system,sans-serif;background:#070b1e;background-image:radial-gradient(1px 1px at 10% 20%,#ffffff22 1px,transparent 0),radial-gradient(1px 1px at 30% 65%,#ffffff18 1px,transparent 0),radial-gradient(1.5px 1.5px at 55% 15%,#ffffff28 1px,transparent 0),radial-gradient(1px 1px at 75% 80%,#ffffff15 1px,transparent 0),radial-gradient(1px 1px at 90% 40%,#ffffff20 1px,transparent 0);color:#e8e8f0;display:flex;justify-content:center;align-items:center;min-height:100vh}.container{background:#0a0e27;border:1px solid #1e2550;border-radius:12px;box-shadow:0 4px 20px rgba(0,0,0,.4);padding:40px;text-align:center;max-width:480px;width:90%}h2{color:#f0b800;margin-bottom:32px;font-size:22px;font-weight:500}.btn{display:inline-block;padding:14px 28px;border:none;border-radius:8px;font-size:16px;cursor:pointer;min-width:220px;margin:8px 0;font-weight:600;transition:all .2s}.btn-primary{background:#f0b800;color:#070b1e}.btn-primary:hover{background:#e8a800}.btn-secondary{background:#111638;color:#e8e8f0;border:1px solid #1e2550}.btn-secondary:hover{background:#1e2550}</style></head><body><div class="container"><h2>Backup & Restore</h2><button class="btn btn-primary" id="backupButton">Download Backup</button><br><input type="file" id="fileInput" style="display:none"><button class="btn btn-secondary" id="restoreButton">Upload for Restore</button></div><script>function trimLeadingSlash(path){return path.startsWith('/') ? path.slice(1) : path;}function joinPaths(...paths){return paths.join('/').replace(/\/+/g, '/');}async function getFilesFromDirectory(dir, zip){const response=await fetch(`/list?dir=${dir}`); const file_list=await response.json(); for (let file_info of file_list){if (file_info['type']==='file'){const filename=file_info['name']; const source_file_url=`${dir}${filename}`; const fileResponse=await fetch(source_file_url); if (fileResponse.status===200){const fileContent=await fileResponse.blob(); zip.file(trimLeadingSlash(joinPaths(dir, filename)), fileContent);}else{alert(`Failed to download file ${filename}`);}}else if (file_info['type']==='dir'){await getFilesFromDirectory(`${dir}${file_info['name']}/`, zip);}}}document.getElementById('backupButton').addEventListener('click', async function (){let zip=new JSZip(); let btn=document.getElementById('backupButton'); btn.textContent='Downloading...'; await getFilesFromDirectory('/', zip); zip.generateAsync({type: "blob"}).then(function (blob){const url=window.URL.createObjectURL(blob); const a=document.createElement('a'); a.style.display='none'; a.href=url; a.download='backup.zip'; document.body.appendChild(a); a.click(); window.URL.revokeObjectURL(url);}); btn.textContent='Download Backup';}); document.getElementById('restoreButton').addEventListener('click', function (){document.getElementById('fileInput').click();}); document.getElementById('fileInput').addEventListener('change', async function (){const file=this.files[0]; const zip=new JSZip(); let btn=document.getElementById('restoreButton'); btn.textContent='Restoring. Please wait...'; setTimeout(async ()=>{try{const contents=await zip.loadAsync(file); const uploadPromises=[]; for (let filename in contents.files){if (!contents.files[filename].dir){const fileContent=await contents.files[filename].async("blob"); const target_file_url=`/edit?filename=${filename}`; const formData=new FormData(); formData.append('file', fileContent, filename); const uploadPromise=fetch(target_file_url,{method: 'POST', body: formData}); uploadPromises.push(uploadPromise);}}const responses=await Promise.all(uploadPromises); if (responses.every(response=> response.ok)){btn.textContent='Upload for Restore'; fetch('/api/reboot',{method: 'POST'}); alert(`Backup successfully restored. Rebooting..`);}else{alert(`Failed to restore backup.`); btn.textContent='Upload for Restore';}}catch (error){alert(`An error occurred: ${error}`); btn.textContent='Upload for Restore';}}, 10);}); </script></body></html>

)EOF";


static const char screenfull_html[] PROGMEM = R"EOF(
<!doctype html><html> <head> <title>SVITRIX - Fullscreen</title> <style>body{display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; overflow: hidden; background: #070b1e;}canvas{display: block; width: 100vw; background: #000; z-index: 1;}</style> </head> <body><canvas id=c></canvas></body> <script>const c=document.getElementById("c"), d=c.getContext("2d");const urlParams=new URLSearchParams(window.location.search);const queriedFPS=parseInt(urlParams.get('fps'));let fps=%%FPS%%;function scd(){const t=window.innerWidth; c.width=t, c.height=t / 4;}function j(){fetch("/api/screen").then(t=> t.json()).then(t=>{d.clearRect(0, 0, c.width, c.height); d.fillStyle="#000"; for (let e=0; e < 8; e++) for (let n=0; n < 32; n++){const i=t[32 * e + n], o=(16711680 & i) >> 16, s=(65280 & i) >> 8, h=255 & i; d.fillStyle=`rgb(${o},${s},${h})`; d.fillRect(n * (c.width / 32), e * (c.height / 8), c.width / 32 - 4, c.height / 8 - 4);}setTimeout(j, 1000 / fps);});}scd();document.addEventListener("DOMContentLoaded", j);window.addEventListener("resize", scd); </script></html>

)EOF";


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
