### Project Setup
The project file is included, but if you create it from scratch, make sure to add the following change.
In build events, add this line to the Post-Build Event list:

```xcopy "$(ProjectDir)view" "$(TargetDir)view" /E/H/C/I/Y```

### Running
This is the command for the executable:

```StaticHttpServer [-u {hostUrl}] [-d {hostDir}]```
* the `hostUrl` and `hostDir` arguments are optional
* `hostUrl` is the base url the server will attempt to bind to
* `hostDir` is the relative or absolute path to the directory the contents will be fetched from