# XMan
Extension Management for REDCap. Provides an easy way to enable, disable and configure external extensions on REDCap projects.

It is based on [Drupal 7 modules architecture](https://www.drupal.org/docs/7/creating-custom-modules).

## Installation
- Download XMan and drop `xman` folder in your REDCap root
- Move `.htaccess` file to the REDCap root
- Go to Control *Center > General Configuration*
- Set *REDCap Hooks* as the absolute path to the `includes/hooks.inc` file (e.g. `/var/www/redcap/xman/includes/hooks.inc`), and save

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

### .extension.info file
The .extension file is a PHP code which consists the heart of the extension. If the extension is enabled, its .extension file included on every page load. Use your .extension file to implement hooks and plugins. Example:

```php
<?php

/**
 * Hook example.
 */
function test_redcap_every_page_top($project_id) {
    print '<script>alert('Test!')</script>';
}

/**
 * Plugin example.
 */
 function test_xman_plugins() {
     return array('Test' => 'test_page');
 }
 
 /**
  * Plugin page callback.
  */
 function test_page() {
     print 'Test!';
 }
```

The code above displays a "Test!" popup on every page. It also creates a new page at `/redcap/Test` that displays the same message.

See details about hooks and plugins implementation on the next sections.

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
You may manage extensions by accessing *Control Center > Extension Manager (XMan)*.

If you followed the example from the previous section, you might see an extension called "Test" on the list. After enabling it, a "Test!" alert will pop up on every page you access (except on pages within the project scope, because this extension is not set as global on .info file). You may also access `/redcap/Test` page, which also display a "Test!" message.

To enable your extension on a given project, access your project main page, and then click on *Extension Manager (XMan)*. This page is analogous the previous one at the Control Center. After enabling "Test" for the given project, now you should see the popup again.

## How implement a hook from an extension
To implement a hook from an extension, open your `<extension_name>.extension` file, and then create a function as follows:

```
function <extension_name>_<hook_name>($param1, $param2, ...) {
}
```

See example on the [Getting started][#getting-started] section - `test_redcap_every_page_top()`.

Check REDCap documentation to see a full list of available hooks. On [New hooks available](#new-hooks-available), there is a list of additional hooks provided by XMan.

### Concurrent hooks
Two (or more) different extensions can implement the **same** hook without any conflicts. But who gets priority on execution? On `xman_extensions` table there is a "weight" column, responsible for sorting the extensions to be executed - the haviest the extension, the lowest priority. Thus, if you need that you extension runs *after* or *before* a concurrent one, you might use one of the hooks listed on the next section to adjust your extension's "weight" value.

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

### hook_xman_update_N()
Triggered when the administrative user submits the "Available updates" form on *Control Center > Extensions Manager (XMan)* page.

This implementation requires an arbitrary version number, e.g. `test_xman_update_1`, `test_xman_update_15`. etc. When triggered, the updater checks if N is bigger than the last executed update, then it gets executed. After that, to perform another update, you need to create a function with a bigger N.

This architecture provides consistency among several users, using different software versions that want to update the code and then run required updates by the admin UI.

Obs.: It is highly recommended to add a comment block above your function header, since it will be displayed as helper/description text on the "Available updates" list.

#### hook_xman_plugins()
Used to declare new plugins. See details on the next section.

## How to create plugins from an extension
Creating plugins from an extension requires implementing `hook_xman_plugins()`. This hook expects a list of page callbacks (i.e. function names responsible for displaying your page contents), keyed by the page path. See the plugin example from [Getting started](#getting-started) section - `test_xman_plugins()`.

You might noticed that **we no not need to create files anymore** to add new plugins, since we are using page callbacks instead. And you also might noticed that we are free to set up any path we want, even outside the extension folder (as soon as the path does not exist yet).

Obs.: Of course, you may also create .php files to be directly accessed within your extension, without passing through `hook_xman_plugins()`. But note that:
- You won't be able to set up a path outside your extension folder
- The page will be available even if your extension is disabled

## Performing updates
If your extension needs some db adjustments after a code update, you might implement a `hook_update_N()` (to know how to do that, see [New hooks available][#new-hooks-available] section).

After implementing your update function, go to *Control Center > Extensions Manager (Xman)*. Then you should see your update listed on "Available updates" section.
