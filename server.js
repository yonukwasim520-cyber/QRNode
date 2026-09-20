const express = require("express");

const app = express();
const PORT = 6000;

app.get("/", (req, res) => {
    res.send("QRNode يعمل بنجاح!");
});

app.listen(PORT, "0.0.0.0", () => {
    console.log(`Server running at http://127.0.0.1:${PORT}`);
});
