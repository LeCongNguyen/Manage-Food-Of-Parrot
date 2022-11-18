<!DOCTYPE html>
<html>

<head>
    <title>Manage Food Of Parrot</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta.2/css/bootstrap.min.css">
    <link rel="stylesheet" href="./css/index.css">
    <?php
    $jsonString = file_get_contents("./feeder-status.json");
    $data = json_decode($jsonString, true);
    if (isset($_GET["keo"])) {
        $data["keo"] = $_GET["keo"];
        $newJsonString = json_encode($data);
        file_put_contents("./feeder-status.json", $newJsonString);
    }
    if (isset($_GET["keoSet"])) {
        $data["keoSet"] = $_GET["keoSet"];
        $newJsonString = json_encode($data);
        file_put_contents("./feeder-status.json", $newJsonString);
    }
    if (isset($_GET["done"])) {
        $data["done"] = $_GET["done"];
        $newJsonString = json_encode($data);
        file_put_contents("./feeder-status.json", $newJsonString);
    }
    ?>
</head>

<body>
    <div class="container-fluid">
        <div class="row">
            <div id='KEO' class='parrot col-12'>
                <div class='card top-of-card card-bg-color'>
                    <div class='card-body'>
                        <h3 class='card-title'>
                            <b>KẸO</b>
                            <button class="setting-btn" type="button" data-toggle="modal" data-target="#keo-setting">
                                <img class="icon setting-btn" src="./imgs/icons/setting-icon.png">
                            </button>
                            <!-- settingModal -->
                            <div class="modal" id="keo-setting">
                                <div class="modal-dialog">
                                    <div class="modal-content">
                                        <!-- Modal Header -->
                                        <div class="modal-header">
                                            <h4 class="modal-title setting-title">Keo Setting</h4>
                                            <button type="button" class="close" data-dismiss="modal">&times;</button>
                                        </div>
                                        <!-- Modal body -->
                                        <div class="modal-body setting-body">
                                            <form action="index.php" method="get">
                                                <?php
                                                echo "<input id='keo-setting' type='number' min='0' class='setting-input' name='keoSet' value=" . $data['keoSet'] . "></input>";
                                                ?>
                                                <button type="submit" class="btn btn-danger ok-btn">OK</button>
                                            </form>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </h3>
                        <form action='index.php' method='get'>
                            <?php
                            if ($data['keo'] == 'on') {
                                echo "<input type='text' name='done' value='0'></input>";
                                echo "<input type='text' name='keo' value='off'></input>";
                                echo "<button id='keo-btn' class='btn on-btn' type='submit'>ON</button>";
                            }
                            if ($data['keo'] == 'off') {
                                echo "<input type='text' name='done' value='0'></input>";
                                echo "<input type='text' name='keo' value='on'></input>";
                                echo "<button id='keo-btn' class='btn off-btn' type='submit'>OFF</button>";
                            }
                            ?>
                        </form>
                    </div>
                </div>
            </div>
        </div>

        <script src="https://code.jquery.com/jquery-3.2.1.slim.min.js"></script>
        <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js"></script>
        <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js"></script>
</body>

</html>