#Requires -Version 5

# args
param (
    [Parameter(Mandatory)][ValidateSet('COPY', 'SOURCEGEN', 'DISTRIBUTE')][string]$Mode,
    [string]$Version,
    [string]$Path,
    [string]$Project
)


$ErrorActionPreference = "Stop"

$Folder = $PSScriptRoot | Split-Path -Leaf
$SourceExt = @('.c', '.cpp', '.cxx', '.h', '.hpp', '.hxx')
$ConfigExt = @('.ini', '.json', '.toml')
$env:ScriptCulture = (Get-Culture).Name -eq 'zh-CN'
$env:Detached = $false

function Resolve-Files {
    param (
        [Parameter(ValueFromPipeline)][string]$a_parent = $PSScriptRoot,
        [string[]]$a_directory = @('include', 'src', 'test')
    )
    
    process {
        Push-Location $PSScriptRoot
        $_generated = [System.Collections.ArrayList]::new()

        try {
            foreach ($directory in $a_directory) {
                if (!$env:RebuildInvoke) {
                    Write-Host "`t[$a_parent/$directory]"
                }

                Get-ChildItem "$a_parent/$directory" -Recurse -File -ErrorAction SilentlyContinue | Where-Object {
                    ($_.Extension -in $SourceExt) -and 
                    ($_.Name -ne 'Version.h')
                } | Resolve-Path -Relative | ForEach-Object {
                    if (!$env:RebuildInvoke) {
                        Write-Host "`t`t<$_>"
                    }
                    $_generated.Add("`n`t`"$($_.Substring(2) -replace '\\', '/')`"") | Out-Null
                }
            }               
            
            Get-ChildItem "$a_parent" -File -ErrorAction SilentlyContinue | Where-Object {
                ($_.Extension -in $ConfigExt) -and 
                ($_.Name -notmatch 'cmake|vcpkg')
            } | Resolve-Path -Relative | ForEach-Object {
                if (!$env:RebuildInvoke) {
                    Write-Host "`t`t<$_>"
                }
                $_generated.Add("`n`t`"$($_.Substring(2) -replace '\\', '/')`"") | Out-Null
            }
        } finally {
            Pop-Location
        }

        return $_generated
    }
}


Write-Host "`n`t<$Folder> [$Mode]"


# @@COPY
if ($Mode -eq 'COPY') {

    if (!$env:Detached) {
        $env:Detached = $true
        Start-Process cmd.exe -ArgumentList "/k powershell -NoProfile -ExecutionPolicy Bypass -File `"$PSScriptRoot/!Update.ps1`" `"COPY`" `"$Version`" `"$Path`" `"$Project`""
        Exit
    }

    # process newly added files
    $BuildFolder = Get-ChildItem (Get-Item $Path).Parent.Parent.FullName "$Project.sln" -Depth 2 -File -Exclude ('*CMakeFiles*', '*CLib*')
    $NewFiles = Get-ChildItem $BuildFolder.DirectoryName -File | Where-Object {$_.Extension -in $SourceExt}
    if ($NewFiles) { # trigger ZERO_CHECK
        $NewFiles | Move-Item -Destination "$PSScriptRoot/src" -Force -Confirm:$false -ErrorAction:SilentlyContinue | Out-Null
        [IO.File]::WriteAllText("$PSScriptRoot/CMakeLists.txt", [IO.File]::ReadAllText("$PSScriptRoot/CMakeLists.txt"))
    }

    # Build Target
    Write-Host "`t$Folder $Version"
    $vcpkg = [IO.File]::ReadAllText("$PSScriptRoot/vcpkg.json") | ConvertFrom-Json
    $Install = $vcpkg.'features'.'mo2-install'.'description'
    $ProjectCMake = [IO.File]::ReadAllText("$PSScriptRoot/CMakeLists.txt")
    $OldVersion = [regex]::match($ProjectCMake, '(?s)(?:(?<=\sVERSION\s)(.*?)(?=\s+))').Groups[1].Value


    function Copy-Mod {
        param (
            $Data
        )

        New-Item -Type Directory "$Data/SKSE/Plugins" -Force | Out-Null

        # binary
        Copy-Item "$Path/$Project.dll" "$Data/SKSE/Plugins/$Project.dll" -Force
        $Message.Text += "`r`n- Binary files copied"

        # configs
        Get-ChildItem $PSScriptRoot | Where-Object {
            ($_.Extension -in $ConfigExt) -and 
            ($_.Name -notmatch 'CMake|vcpkg')
        } | ForEach-Object {
            Copy-Item $_.FullName "$Data/SKSE/Plugins/$($_.Name)" -Force
            $Message.Text += "`r`n- Configuration files copied"
        }

        # shockwave
        if (Test-Path "$PSScriptRoot/Interface/*.swf" -PathType Leaf) {
            New-Item -Type Directory "$Data/Interface" -Force | Out-Null
            Copy-Item "$PSScriptRoot/Interface" "$Data" -Recurse -Force
            $Message.Text += "`r`n- Shockwave files copied"
        }

        # papyrus
        if (Test-Path "$PSScriptRoot/Scripts/*.pex" -PathType Leaf) {
            New-Item -Type Directory "$Data/Scripts" -Force | Out-Null
            xcopy.exe "$PSScriptRoot/Scripts" "$Data/Scripts" /C /I /S /E /Y
            $Message.Text += "`r`n- Papyrus scripts copied"
        }
        if (Test-Path "$PSScriptRoot/Scripts/Source/*.psc" -PathType Leaf) {
            New-Item -Type Directory "$Data/Scripts/Source" -Force | Out-Null
            xcopy.exe "$PSScriptRoot/Scripts/Source" "$Data/Scripts/Source" /C /I /S /E /Y
            $Message.Text += "`r`n- Papyrus scripts copied"
        }
    }


	Add-Type -AssemblyName Microsoft.VisualBasic
    Add-Type -AssemblyName System.Windows.Forms
    Add-Type -AssemblyName System.Drawing

    [System.Windows.Forms.Application]::EnableVisualStyles()
    $MsgBox = New-Object System.Windows.Forms.Form -Property @{
        TopLevel = $true
        ClientSize = '350, 305'
        Text = $Project
        StartPosition = 'CenterScreen'
        FormBorderStyle = 'FixedDialog'
        MaximizeBox = $false
        MinimizeBox = $false
        Font = New-Object System.Drawing.Font('Segoe UI', 10, [System.Drawing.FontStyle]::Regular)
    }
    
    $Message = New-Object System.Windows.Forms.TextBox -Property @{
        ClientSize = '225, 150'
        Location = New-Object System.Drawing.Point(20, 20)
        Multiline = $true
        ReadOnly = $true
        Text = "- [$Project - $OldVersion] has been built."
        
    }
    
    $BtnCopyMO2 = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Location = New-Object System.Drawing.Point(260, 19)
        Text = 'Copy to MO2'
        Add_Click = {
            $BtnCopyMO2.Enabled = $false
            foreach ($runtime in @("$($env:MO2SkyrimAEPath)/mods", "$($env:MO2SkyrimSEPath)/mods", "$($env:MO2SkyrimVRPath)/mods")) {
                if (Test-Path $runtime -PathType Container) {
                    Copy-Mod "$runtime/$Install"
                }
            }
            $Message.Text += "`r`n- Copied to MO2."
            $BtnCopyMO2.Enabled = $true
        }
    }
    
    $BtnCopyData = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Location = New-Object System.Drawing.Point(260, 74)
        Text = 'Copy to Data'
        Add_Click = {
            $BtnCopyData.Enabled = $false
            foreach ($runtime in @("$($env:SkyrimAEPath)/data", "$($env:SkyrimSEPath)/data", "$($env:SkyrimVRPath)/data")) {
                if (Test-Path $runtime -PathType Container) {
                    Copy-Mod "$runtime"
                }
            }
            $Message.Text += "`r`n- Copied to game data."
            $BtnCopyData.Enabled = $true
        }
    }
    
    $BtnRemoveData = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Text = 'Remove in Data'
        Location = New-Object System.Drawing.Point(260, 129)
        Add_Click = {
            $BtnRemoveData.Enabled = $false
            foreach ($runtime in @("$($env:SkyrimAEPath)/data", "$($env:SkyrimSEPath)/data", "$($env:SkyrimVRPath)/data")) {
                if (Test-Path "$runtime/SKSE/Plugins/$Project.dll" -PathType Leaf) {
                    Remove-Item "$runtime/SKSE/Plugins/$Project.dll" -Force -Confirm:$false -ErrorAction:SilentlyContinue | Out-Null
                }
            }
            $Message.Text += "`r`n- Removed from game data."
            $BtnRemoveData.Enabled = $true
        }
    }
    
    $BtnOpenFolder = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Text = 'Show in Explorer'
        Location = New-Object System.Drawing.Point(260, 185)
        Add_Click = {
            Invoke-Item $Path
        }
    }
    
    $BtnLaunchSKSEAE = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Text = 'SKSE (AE)'
        Location = New-Object System.Drawing.Point(20, 185)
        Add_Click = {
            $BtnLaunchSKSEAE.Enabled = $false

            Push-Location $env:SkyrimAEPath
            Start-Process ./SKSE64_loader.exe
            Pop-Location

            $Message.Text += "`r`n- SKSE (AE) Launched."
            $BtnLaunchSKSEAE.Enabled = $true
        }
    }
    if (!(Test-Path "$env:SkyrimAEPath/skse64_loader.exe" -PathType Leaf)) {
        $BtnLaunchSKSEAE.Enabled = $false
    }

    $BtnLaunchSKSESE = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Text = 'SKSE (SE)'
        Location = New-Object System.Drawing.Point(100, 185)
        Add_Click = {
            $BtnLaunchSKSESE.Enabled = $false

            Push-Location $env:SkyrimSEPath
            Start-Process ./SKSE64_loader.exe
            Pop-Location

            $Message.Text += "`r`n- SKSE (SE) Launched."
            $BtnLaunchSKSESE.Enabled = $true
        }
    }
    if (!(Test-Path "$env:SkyrimSEPath/skse64_loader.exe" -PathType Leaf)) {
        $BtnLaunchSKSESE.Enabled = $false
    }
 
    $BtnLaunchSKSEVR = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Text = 'SKSE (VR)'
        Location = New-Object System.Drawing.Point(180, 185)
        Add_Click = {
            $BtnLaunchSKSEVR.Enabled = $false

            Push-Location $env:SkyrimVRPath
            Start-Process ./SKSE64_loader.exe
            Pop-Location

            $Message.Text += "`r`n- SKSE (VR) Launched."
            $BtnLaunchSKSEVR.Enabled = $true
        }
    }
    if (!(Test-Path "$env:SkyrimVRPath/skse64_loader.exe" -PathType Leaf)) {
        $BtnLaunchSKSEVR.Enabled = $false
    }
    
    $BtnBuildPapyrus = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Text = 'Build Papyrus'
        Location = New-Object System.Drawing.Point(20, 240)
        Add_Click = {
            $BtnBuildPapyrus.Enabled = $false
            $BtnBuildPapyrus.Text = 'Compiling...'
            
            $Invocation = "`"$($env:SkyrimSEPath)/Papyrus Compiler/PapyrusCompiler.exe`" `"$PSScriptRoot/Scripts/Source`" -f=`"$env:SkyrimSEPath/Papyrus Compiler/TESV_Papyrus_Flags.flg`" -i=`"$env:SkyrimSEPath/Data/Scripts/Source;$PSScriptRoot/Scripts;$PSScriptRoot/Scripts/Source`" -o=`"$PSScriptRoot/Scripts`" -a -op -enablecache -t=`"4`""
            Start-Process cmd.exe -ArgumentList "/k $Invocation && pause && exit"
            
            $BtnBuildPapyrus.Text = 'Build Papyrus'
            $BtnBuildPapyrus.Enabled = $true
        }
    }
    
    $BtnChangeVersion = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Text = 'Version'
        Location = New-Object System.Drawing.Point(100, 240)
        Add_Click = {
            $BtnChangeVersion.Enabled = $false
            $NewVersion = $null
            while ($OldVersion -and !$NewVersion) {
                $NewVersion = [Microsoft.VisualBasic.Interaction]::InputBox("Input the new versioning for $Project", 'Versioning', $OldVersion)
            }
            $ProjectCMake = $ProjectCMake -replace "VERSION\s$OldVersion", "VERSION $NewVersion"
            $vcpkg.'version-string' = $NewVersion

            [IO.File]::WriteAllText("$PSScriptRoot/CMakeLists.txt", $ProjectCMake)
            $vcpkg = $vcpkg | ConvertTo-Json -Depth 9
            [IO.File]::WriteAllText("$PSScriptRoot/vcpkg.json", $vcpkg)


            $Message.Text += "`r`n- Version changed $OldVersion -> $NewVersion"
            $OldVersion = $NewVersion

            $BtnChangeVersion.Enabled = $true
        }
    }
    
    $BtnPublish = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Text = 'Publish Mod'
        Location = New-Object System.Drawing.Point(180, 240)
        Add_Click = {
            $BtnPublish.Enabled = $false
            $BtnPublish.Text = 'Zipping...'

            Copy-Mod "$PSScriptRoot/Tmp/Data"
            Compress-Archive "$PSScriptRoot/Tmp/Data/*" "$Path/$($Project)-$(($OldVersion).Replace('.', '-'))" -Force
            Remove-Item "$PSScriptRoot/Tmp" -Recurse -Force -Confirm:$false -ErrorAction:SilentlyContinue | Out-Null
            Invoke-Item $Path

            $Message.Text += "`r`n- Mod files zipped & ready to go."
            $BtnPublish.Text = 'Publish Mod'
            $BtnPublish.Enabled = $true
        }
    }
    
    
    $BtnExit = New-Object System.Windows.Forms.Button -Property @{
        ClientSize = '70, 50'
        Text = 'Exit'
        Location = New-Object System.Drawing.Point(260, 240)
        Add_Click = {
            $MsgBox.Close()
        }
    }
                
    $MsgBox.Controls.Add($Message)
    $MsgBox.Controls.Add($BtnCopyData)
    $MsgBox.Controls.Add($BtnCopyMO2)
    $MsgBox.Controls.Add($BtnRemoveData)
    $MsgBox.Controls.Add($BtnOpenFolder)
    $MsgBox.Controls.Add($BtnExit)
    $MsgBox.Controls.Add($BtnBuildPapyrus)
    $MsgBox.Controls.Add($BtnChangeVersion)
    $MsgBox.Controls.Add($BtnPublish)
    $MsgBox.Controls.Add($BtnLaunchSKSEAE)
    $MsgBox.Controls.Add($BtnLaunchSKSESE)
    $MsgBox.Controls.Add($BtnLaunchSKSEVR)
    
    $MsgBox.ShowDialog() | Out-Null
    Exit
}


# @@SOURCEGEN
if ($Mode -eq 'SOURCEGEN') {
    Write-Host "`tGenerating CMake sourcelist..."
    Remove-Item "$Path/sourcelist.cmake" -Force -Confirm:$false -ErrorAction Ignore

    $generated = 'set(SOURCES'
    $generated += $PSScriptRoot | Resolve-Files
    if ($Path) {
        $generated += $Path | Resolve-Files
    }
    $generated += "`n)"
    [IO.File]::WriteAllText("$Path/sourcelist.cmake", $generated)
}


# @@DISTRIBUTE
if ($Mode -eq 'DISTRIBUTE') { # update script to every project
    Get-ChildItem "$PSScriptRoot/*/*" -Directory | Where-Object {
        $_.Name -notin @('vcpkg', 'Build', '.git', '.vs') -and
        (Test-Path "$_/CMakeLists.txt" -PathType Leaf)
    } | ForEach-Object {
        Write-Host "`tUpdated <$_>"
        Robocopy.exe "$PSScriptRoot" "$_" '!Update.ps1' /MT /NJS /NFL /NDL /NJH | Out-Null
    }
}



