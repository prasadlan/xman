<?php
/**
 * @file
 * XMan administrative pages callbacks.
 */

/**
 * Displays XMan configuration page on Control Center.
 */
function xman_control_center_page() {
    // Bringing global variables to the scope of this function.
    extract($GLOBALS);

    include APP_PATH_DOCROOT . 'ControlCenter/header.php';
    xman_form();
    include APP_PATH_DOCROOT . 'ControlCenter/footer.php';
}

/**
 * Displays XMan configuration page on project level.
 */
function xman_project_page() {
    // Bringing global variables to the scope of this function.
    extract($GLOBALS);

    require_once APP_PATH_DOCROOT . 'Config/init_project.php';
    require_once APP_PATH_DOCROOT . 'ProjectGeneral/form_renderer_functions.php';

    include APP_PATH_DOCROOT . 'ProjectGeneral/header.php';
    xman_form(PROJECT_ID, false);
    include APP_PATH_DOCROOT . 'ProjectGeneral/footer.php';
}

/**
 * Builds XMan configuration form.
 */
function xman_form($project_id = null, $include_update_form = true) {
    $extensions = $project_id ? xman_get_enabled_extensions(null, false) : xman_get_extensions();

    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        $class = 'darkgreen';
        $icon = 'tick';

        if ($_POST['form_type'] == 'save') {
            $to_del = array();
            foreach ($extensions as $name => $extension) {
                if (!empty($_POST[$name])) {
                    if ($project_id) {
                        if (empty($extension['projects'][$project_id])) {
                            $extensions[$name]['projects'][$project_id] = $project_id;
                            xman_enable_extension($name, $project_id);
                        }
                    }
                    elseif (!$extenion[$enabled]) {
                        $extensions[$name]['enabled'] = 1;
                        xman_enable_extension($name);
                    }
                }
                elseif ($project_id) {
                    if (!empty($extensions[$name]['projects'][$project_id])) {
                        unset($extensions[$name]['projects'][$project_id]);
                        $to_del[] = $name;
                    }
                }
                else {
                    $extensions[$name]['enabled'] = 0;
                    $to_del[] = $name;
                }
            }

            if (!empty($to_del)) {
                xman_disable_extensions($to_del, $project_id);
            }

            $msg = 'Your configuration has been saved successfully.';
        }
        elseif ($include_update_form && $_POST['form_type'] == 'update') {
            $msg = 'The updates have been applied successfully.';
            foreach (xman_get_available_updates() as $name => $updates) {
                if (!empty($_POST[$name]) && !xman_update_extension($name, $updates)) {
                    $msg = 'An error has ocurred. Please try again or contact site administration.';
                    $class = 'red';
                    $icon = 'cross';

                    break;
                }
            }
        }

        print '<div class="' . $class . '"><img src="' . APP_PATH_IMAGES . $icon . '.png"> ' . $msg . '</div>';
    }
?>

<h4><img src="<?php print APP_PATH_IMAGES; ?>brick.png"> Extensions</h4>
<?php if ($extensions): ?>
    <form action="<?php print PAGE_FULL; ?>" enctype="multipart/form-data" target="_self" method="post" name="xman-save-form" id="xman-save-form" style="max-width: 700px;">
        <input type="hidden" name="form_type" value="save">
        <table class="table">
            <tr>
                <th>Name</th>
                <th>Description</th>
                <?php if (!$project_id): ?>
                    <th>Type</th>
                <?php endif; ?>
                <th>Version</th>
                <th>Authors</th>
            </tr>
            <?php foreach ($extensions as $name => $info): ?>
                <tr>
                    <td style="min-width:120px;">
                        <?php $enabled = $project_id ? !empty($info['projects'][$project_id]) : $info['enabled']; ?>
                        <input type="checkbox" id="<?php print $name; ?>" name="<?php print $name; ?>" value="enabled"<?php print $enabled ? ' checked' : ''; ?>>
                        <label for="<?php print $name; ?>"><?php print $info['name']; ?></label>
                    </td>
                    <td style="min-width:120px;"><?php print empty($info['description']) ? '-' : htmlspecialchars($info['description']); ?></td>
                    <?php if (!$project_id): ?>
                        <td><?php print empty($info['global']) ? 'Project' : 'Global'; ?></td>
                    <?php endif; ?>
                    <td><?php print empty($info['version']) ? '-' : $info['version']; ?></td>
                    <td><?php print empty($info['authors']) ? '-' : htmlspecialchars($info['authors']); ?></td>
                </tr>
            <?php endforeach; ?>
        </table>
        <div class="text-center">
            <button class="btn btn-success" type="submit">Save</button>
        </div>
    </form>

    <?php if ($include_update_form && ($updates = xman_get_available_updates())): ?>
        <h4><img src="<?php print APP_PATH_IMAGES; ?>arrow_refresh.png"> Available extension updates</h4>
        <form action="<?php print PAGE_FULL; ?>" enctype="multipart/form-data" target="_self" method="post" name="xman-update-form" id="xman-update-form" style="max-width: 700px;">
            <input type="hidden" name="form_type" value="update">
            <table class="table">
                <tr>
                    <th>Extension</th>
                    <th>Updates</th>
                </tr>
                <?php foreach ($updates as $name => $updates): ?>
                    <tr>
                        <?php $list = array(); ?>
                        <?php foreach ($updates as $version => $description): ?>
                            <?php $list[] = '#' . $version . ': ' . ($description ? $description : 'No description available.'); ?>
                        <?php endforeach; ?>
                        <td>
                            <input type="checkbox" id="<?php print $name; ?>" name="<?php print $name; ?>" value="checked">
                            <label for="<?php print $name; ?>"><?php print $extensions[$name]['name']; ?></label>
                        </td>
                        <td><?php print implode('<br>', $list); ?>
                    </tr>
                <?php endforeach; ?>
            </table>
            <div class="text-center">
                <button class="btn btn-primary" type="submit">Run updates</button>
            </div>
        </form>
    <?php endif; ?>
<?php else: ?>
    <div class="text-info">No extensions available.</div>
<?php endif; ?>
<?php
}
