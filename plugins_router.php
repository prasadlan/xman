<?php
/**
 * @file
 * XMan path router.
 */

require '../redcap_connect.php';

if (($pages = xman_get_pages()) && $_GET['q']) {
    $path = $_GET['q'];

    if (!empty($pages[$path]) && function_exists($pages[$path])) {
        $pages[$path]();
        exit;
    }
}

http_response_code(404);
exit('Page not found');
