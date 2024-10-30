<?php
$filename = 'results.txt';

// Default values for LED control and RGB
$ledControl = "LED Control: OFF"; // Default to OFF if not set
$rgbValues = "RGB: 0, 0, 0";      // Default RGB values

// Read current values from the file, if it exists
if (file_exists($filename)) {
    $content = file_get_contents($filename);
    preg_match('/LED Control: (ON|OFF)/', $content, $ledMatch);
    preg_match('/RGB: (\d+), (\d+), (\d+)/', $content, $rgbMatch);

    if ($ledMatch) {
        $ledControl = $ledMatch[0]; // Use the current LED control state
    }
    if ($rgbMatch) {
        $rgbValues = $rgbMatch[0]; // Use the current RGB values
    }
}

// Update LED control if specified
if (isset($_GET['ledControl'])) {
    $ledControl = "LED Control: " . $_GET['ledControl'];
}

// Update RGB values if specified
if (isset($_GET['r']) && isset($_GET['g']) && isset($_GET['b'])) {
    $r = intval($_GET['r']);
    $g = intval($_GET['g']);
    $b = intval($_GET['b']);
    $rgbValues = "RGB: $r, $g, $b";
}

// Write the updated values back to the file
$content = "$ledControl\n$rgbValues";
file_put_contents($filename, $content);

echo "File updated with current LED state and RGB values.";
?>
