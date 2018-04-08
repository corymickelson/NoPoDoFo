export function setup() {
    const libSource = `${process.env[ 'LAMBDA_TASK_ROOT' ]}/node_modules/commonpdf_podofo`
    process.env[ 'PATH' ] = `${process.env[ 'PATH' ]}:${libSource}`
    process.env[ 'LD_LIBRARY_PATH' ] = `${libSource}`
}