<?php
/**
 * @file
 * XMan path router.
 *
 * This router is responsible for displaying the pages of extensions plugins.
 * It is done by looking for the "q" query parameter. Example:
 *
 *   /redcap/xman/plugins_router.php?q=<PLUGIN_PATH>
 *
 * However, it is possible - and recommended - to access the plugins directly
 * by its path alias (see .htaccess file to check how this path rewrite works):
 *   /redcap/<PLUGIN_PATH>
 *
 * The router checks if some extension has implemented a page callback for
 * <PLUGIN_PATH> (see Plugins section on README to see how an extension can
 * create a plugin). If so, the function is called and the plugin page is
 * finally rendered.
 */

if (!empty($_GET['q'])) {
    // Defines PAGE_FULL as the plugin path.
    // It avoids REDCap to define it as the router path.
    define('PAGE_FULL', htmlspecialchars($_SERVER['REQUEST_URI']));
    require '../redcap_connect.php';

    // Getting available pages from enabled modules.
    if ($pages = xman_get_pages()) {
        $path = $_GET['q'];
        if (!empty($pages[$path]) && function_exists($pages[$path])) {
            // Calling page callback.
            $pages[$path]();
            exit;
        }
    }
}

if (empty($_GET['not_found'])) {
    // Not found.
    redirect(PAGE_FULL . (count($_GET) > 1 ? '&' : '?') . 'not_found=1');
}
