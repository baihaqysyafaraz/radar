const canvas = document.getElementById('radar');
const ctx = canvas.getContext('2d');
const centerX = canvas.width / 2;
const centerY = canvas.height;
const radius = 500;

function drawRadar(angle, distance, objType) {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    ctx.strokeStyle = '#0f0';
    ctx.lineWidth = 1;

    // Tambahkan lingkaran dan label jarak
    for (let i = 1; i <= 6; i++) {
        let r = i * 60;
        ctx.beginPath();
        ctx.arc(centerX, centerY, r, Math.PI, 2 * Math.PI);
        ctx.stroke();

        // Tambahkan teks label (misal: "10 cm", "20 cm", ...)
        ctx.fillStyle = "#0f0";
        ctx.font = "12px sans-serif";
        ctx.fillText(`${i * 10} cm`, centerX + 5, centerY - r + 10);
    }

    // Tambahkan garis pembagi setiap 30 derajat
    for (let a = 0; a <= 180; a += 30) {
        const rad = a * Math.PI / 180;
        const x = centerX + radius * Math.cos(rad);
        const y = centerY - radius * Math.sin(rad);
        ctx.beginPath();
        ctx.moveTo(centerX, centerY);
        ctx.lineTo(x, y);
        ctx.stroke();
    }

    // Garis sapuan radar
    const sweepRad = angle * Math.PI / 180;
    const sweepX = centerX + radius * Math.cos(sweepRad);
    const sweepY = centerY - radius * Math.sin(sweepRad);

    ctx.strokeStyle = '#0ff';
    ctx.beginPath();
    ctx.moveTo(centerX, centerY);
    ctx.lineTo(sweepX, sweepY);
    ctx.stroke();

    // Titik objek terdeteksi
    const objectX = centerX + distance * 5 * Math.cos(sweepRad); // 1 cm = 5px
    const objectY = centerY - distance * 5 * Math.sin(sweepRad);

    if (objType !== "kosong") {
        ctx.fillStyle = {
            "dekat": "red",
            "sedang": "orange",
            "jauh": "yellow"
        }[objType] || "white";

        ctx.beginPath();
        ctx.arc(objectX, objectY, 6, 0, 2 * Math.PI);
        ctx.fill();

        ctx.fillStyle = "white";
        ctx.font = "12px sans-serif";
        ctx.fillText(objType.toUpperCase(), objectX + 8, objectY);
    }
}

async function getDataAndDraw() {
    try {
        const res = await fetch('/get_data');
        const data = await res.json();
        drawRadar(data.angle, data.distance, data.type);
    } catch (e) {
        console.error("Gagal ambil data", e);
    }
}

setInterval(getDataAndDraw, 200);
