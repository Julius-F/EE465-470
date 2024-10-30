<?php
header('Content-Type: application/json');

$filename = 'results.txt';

// Check if the file exists
if (file_exists($filename)) {
    // Read the contents of the file
    $content = file_get_contents($filename);
    
    // Extract LED control and RGB values using regex
    preg_match('/LED Control: (ON|OFF)/', $content, $ledMatch);
    preg_match('/RGB: (\d+), (\d+), (\d+)/', $content, $rgbMatch);
    
    // Set default values in case parts are missing
    $ledStatus = isset($ledMatch[1]) ? $ledMatch[1] : "OFF";
    $red = isset($rgbMatch[1]) ? intval($rgbMatch[1]) : 0;
    $green = isset($rgbMatch[2]) ? intval($rgbMatch[2]) : 0;
    $blue = isset($rgbMatch[3]) ? intval($rgbMatch[3]) : 0;

    // Create an associative array with the data
    $response = array(
        "LED_Control" => $ledStatus,
        "RGB" => array(
            "R" => $red,
            "G" => $green,
            "B" => $blue
        )
    );

    // Encode the array to JSON and output it
    echo json_encode($response);
} else {
    // Handle error if the file doesn't exist
    echo json_encode(array("error" => "File not found"));
}
?>
