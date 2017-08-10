# XMan
Extension Management for REDCap. Provides an easy way to enable, disable and configure external extensions on REDCap projects.

It is based on [Drupal 7 modules architecture](https://www.drupal.org/docs/7/creating-custom-modules).

## Prerequisites
- Apache's [mod_rewrite](http://httpd.apache.org/docs/current/mod/mod_rewrite.html) extension enabled.

## Installation
- Download XMan and drop `xman` folder in your REDCap root
- Run `install.sql` against your REDCap database
- Move `.htaccess` file to the REDCap root
- Go to **Control Center > General Configuration**, set **REDCap Hooks** as the absolute path to the `includes/hooks.inc` file (e.g. `/var/www/redcap/xman/includes/hooks.inc`), and save

## Getting started
In order to create an extension, you need to add a new folder to `xman/extensions` directory (e.g. `xman/extensions/test`).

An extension requires 2 files to work:
- `<extension_name>.info` (e.g. `test.info`)
- `<extension_name>.extension` (e.g `test.extension`)

Note that the two file names above should be consistent with the folder name. If your extension folder is named `test`, your files should be named as `test.info` and `test.extension`.

### .info file
The .info file is a JSON that contains basic info about your extension. Example:
```
{
    "name": "Test",
    "description": "Prints a test message on every page.",
    "version": "1.0",
    "global": 0,
    "authors": "Joe Doe <example@example.com>"
}
```

Maybe the only confusing property above would be `global`. It defines whether the extension is global or to enabled/disabled on each REDCap project.

### .extension file
The .extension file is the heart of a extension. Once the extension is enabled, its .extension file will included on every page load. Use your .extension file to implement hooks and plugins. Example:

```php
<?php

/**
 * Hook example.
 */
function test_redcap_every_page_top($project_id) {
    print '<script>alert("Test!");</script>';
}

/**
 * Plugin example.
 */
 function test_xman_plugins() {
     return array('Test.php' => 'test_page');
 }
 
 /**
  * Plugin page callback.
  */
 function test_page() {
     print 'Test!';
 }
```

The code above displays a "Test!" popup on every page. It also creates a new page at `/redcap/Test.php` that displays the same message (note that `Test.php` is not an actual file, it is just a path alias).

See details about [hooks](#new-hooks-available) and [plugins](#how-to-create-plugins-from-an-extension) implementation on the next sections.

### (optional) .install file
This PHP file not mandatory, but if you need to perform database operations in order to get your extension to work, that is the proper location to implement some dedicated hooks for that, such as `hook_xman_extension_enable()`, `hook_xman_extension_project_enable()`, `hook_xman_extension_disable()`, `hook_xman_extension_project_disable()` and `hook_xman_update_N()`. Example:

```php
<?php

/**
 * Implements hook_xman_extension_enable().
 */
function test_xman_extension_enable() {
    // Create db tables.
}

/**
 * Implements hook_xman_extension_project_enable().
 */
function test_xman_extension_project_enable($project_id) {
    // Changes project metadata after enabling extension on it.
}

/**
 * Implements hook_xman_extension_disable().
 */
function test_xman_extension_disable() {
    // Delete db tables.
}

/**
 * Adds colunm to db table after changing extension code.
 */
function test_xman_extension_update_1() {
    // Add colunm to db table after changing extension code.
}
```

See [New hooks available](#new-hooks-available) section for further details.

## Managing extensions
You may manage extensions by accessing **Control Center > Extension Manager (XMan)**.

If you followed the example from the previous section, you might see a "Test" extension on the list. After enabling it, a "Test!" alert will pop up on every page you access (except on pages within the project scope, because this extension is not set as global on .info file).

You may also access the plugin page at `/redcap/Test.php`, which also displays a "Test!" message.

To enable your extension on a given project, access your project main page, and then click on **Extension Manager (XMan)**. This page is analogous the previous one at the Control Center. After enabling "Test" for the given project, now you should see the popup again, this time in the project context.

## How implement a hook from an extension
To implement a hook from an extension, open your `<extension_name>.extension` file, and then create a function as follows:

```php
function <extension_name>_<hook_name>($param1, $param2, ...) {
    // Do stuff.
}
```

See example on the [Getting started](#getting-started) section - `test_redcap_every_page_top()`.

Check REDCap documentation to see a full list of available hooks. On [New hooks available](#new-hooks-available) section, there is a list of additional hooks provided by XMan.

### Concurrent hooks
Two (or more) different extensions can implement the **same** hook without any conflicts. But who gets priority on execution? The `xman_extensions` table contains a column named `weight`, responsible for sorting the extensions to be executed - the havier the extension, the lower the priority. Thus, if you need that you extension runs *after* or *before* a concurrent one, you might want to implement `hook_xman_extension_enable()` or `hook_xman_update_N()` to adjust your extension's `weight` value. See [New hooks available](#new-hooks-available) section to know more about these hooks.

## New hooks available
XMan provides custom hooks in order to make sure that the extensions do not need any external script or code or manual intervention to work. They are design to assist the developers on database operations.

#### hook_xman_extension_enable()
Triggered when an extension has been just enabled. Useful for performing any needed db changes (e.g. create db tables required by the extension).

#### hook_xman_extension_project_enable($project_id)
Triggered when an extension has been just enabled in a project. 

#### hook_xman_extension_disable()
Triggered when an extension is about to be disabled. Useful for garbage cleaning (e.g. remove tables created on hook_xman_extension_enable()).

#### hook_xman_extension_project_disable($project_id)
Triggered when an extension is about to be disabled in a project.

#### hook_xman_update_N()
Triggered when the administrative user submits the "Available updates" form on **Control Center > Extensions Manager (XMan)** page.

This implementation requires an arbitrary version number, e.g. `test_xman_update_1`, `test_xman_update_15`. etc. When triggered, the XMan updater checks if N is bigger than the last executed update - if so, the function is executed.

Obs.: It is highly recommended to add a comment block above your function header, since it will be displayed as helper/description text on the "Available updates" list.

#### hook_xman_plugins()
Used to declare new plugins. See details on the next section.

## How to create plugins from an extension
Creating plugins from an extension requires implementing `hook_xman_plugins()`. This hook expects a list of page callbacks (i.e. function names responsible for displaying your page contents), keyed by the page path. See the plugin example from [Getting started](#extension-file) section - `test_xman_plugins()`.

You might have noticed that **we no not need to create files to add new plugins anymore**. And you might also have noticed that we are free to set up any page path we want, even outside the extension folder (as soon as the path does not exist yet).

Obs.: of course, you can also create .php files to be directly accessed within your extension, bypassing the `hook_xman_plugins()` workflow. But note that:
- You lose the flexibility of choose the page path
- The page will be available even if your extension is disabled

## Performing updates
If in the middle of way your extension needs some database adjustments, or if you need to run a version update script, XMan provides a mechanism - called  `hook_xman_update_N()` - to do these things inside your extension (check [this section](#hook_xman_update_n) to know how it works).

After implementing your update function, you should see at  **Control Center > Extensions Manager (Xman)** an "Available updates" section, where you are able to see a list of all pending updates, and execute them.

This way, everytime an user of your extension updates the code, this person will be able to check for updates and run them without any external or special procedure.
