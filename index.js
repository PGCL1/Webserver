// JavaScript function to blink the button's background color
function blinkColor(color) {
    let colorButton = document.getElementById(color);
    let originalColor = colorButton.style.backgroundColor || 'blue';  // Get the original color
    let highlightColor = 'yellow'; // Highlight color for the blink effect

    // Change the button background to the highlight color
    colorButton.style.backgroundColor = highlightColor;

    // After 1 second, change it back to the original color
    setTimeout(() => {
        colorButton.style.backgroundColor = originalColor;
    }, 1000);
}

