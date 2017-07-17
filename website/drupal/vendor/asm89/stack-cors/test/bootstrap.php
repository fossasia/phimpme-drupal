<?php

if (file_exists($file = __DIR__.'/../vendor/autoload.php')) {
    $loader = require_once $file;
    $loader->add('Asm89\Stack', __DIR__);
    $loader->add('Asm89\Stack', __DIR__ . '/../src');
} else {
    throw new RuntimeException('Install dependencies to run test suite.');
}

